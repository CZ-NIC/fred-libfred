/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "libfred/registrable_object/domain/get_domain_state_history.hh"

#include "libfred/registrable_object/history_interval_impl.hh"
#include "libfred/registrable_object/state_flag_setter.hh"
#include "libfred/registrable_object/exceptions_impl.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

namespace {

template <typename T>
DomainStateHistory get_domain_state_history(
        OperationContext& ctx,
        const HistoryInterval& range,
        T get_object_id_rule)
{
    static constexpr auto object_type = Object_Type::domain;
    class LowerLimitVisitor : public boost::static_visitor<std::string>
    {
    public:
        LowerLimitVisitor(Database::query_param_list& params) : params_(params) { }
        std::string operator()(const HistoryInterval::NoLimit&) const
        {
            return "crdate";
        }
        std::string operator()(const ObjectHistoryUuid& history_uuid)const
        {
            return "SELECT valid_from FROM history WHERE uuid=$" + params_.add(history_uuid) + "::UUID";
        }
        std::string operator()(const HistoryInterval::TimePoint<std::chrono::nanoseconds>& at) const
        {
            const auto timestamp_value =
                    SqlConvert<HistoryInterval::TimePoint<std::chrono::nanoseconds>>::to(at);
            return "$" + params_.add(timestamp_value) + "::TIMESTAMP";
        }
    private:
        Database::query_param_list& params_;
    };
    class UpperLimitVisitor : public boost::static_visitor<std::string>
    {
    public:
        UpperLimitVisitor(Database::query_param_list& params) : params_(params) { }
        std::string operator()(const HistoryInterval::NoLimit&) const
        {
            return "'infinity'::TIMESTAMP";
        }
        std::string operator()(const ObjectHistoryUuid& history_uuid) const
        {
            return "SELECT valid_from FROM history WHERE uuid=$" + params_.add(history_uuid) + "::UUID";
        }
        std::string operator()(const HistoryInterval::TimePoint<std::chrono::nanoseconds>& at) const
        {
            const auto timestamp_value =
                    SqlConvert<HistoryInterval::TimePoint<std::chrono::nanoseconds>>::to(at);
            return "$" + params_.add(timestamp_value) + "::TIMESTAMP";
        }
    private:
        Database::query_param_list& params_;
    };
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    const std::string object_id_rule = get_object_id_rule(params);
    const auto lower_limit_rule = boost::apply_visitor(LowerLimitVisitor(params), range.lower_limit);
    const auto upper_limit_rule = boost::apply_visitor(UpperLimitVisitor(params), range.upper_limit);
    const std::string sql =
            "WITH o AS ("
                "SELECT id,type,crdate,erdate,"
                       "(" + lower_limit_rule + ") AS lower_limit,"
                       "(" + upper_limit_rule + ") AS upper_limit "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=(" + object_id_rule + ")) "
            "SELECT o.lower_limit IS NULL OR o.upper_limit IS NULL OR o.upper_limit<o.lower_limit AS \"limit is invalid\","
                   "CASE WHEN o.lower_limit<o.crdate THEN o.crdate ELSE o.lower_limit END AS \"looking start\","
                   "CASE WHEN o.erdate<o.upper_limit THEN o.erdate ELSE o.upper_limit END AS \"looking end\","
                   "os.valid_from<=o.lower_limit AS \"presents on begin\","
                   "os.valid_from,"
                   "os.valid_to,"
                   "eos.name "
            "FROM o "
            "LEFT JOIN object_state os ON os.object_id=o.id AND "
                                         "(o.lower_limit<=os.valid_to OR os.valid_to IS NULL) AND "
                                         "os.valid_from<=o.upper_limit "
            "LEFT JOIN enum_object_states eos ON eos.id=os.state_id AND "
                                                "o.type=ANY(eos.types) "
            "ORDER BY os.valid_from";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        FREDLOG_DEBUG(Conversion::Enums::to_db_handle(object_type) + " does not exist");
        throw ObjectDoesNotExist<object_type>();
    }
    const bool limit_is_invalid = static_cast<bool>(dbres[0][0]);
    if (limit_is_invalid)
    {
        throw InvalidHistoryIntervalSpecification<object_type>();
    }
    const auto upper_limit = static_cast<typename DomainStateHistory::TimePoint>(dbres[0][2]);
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][6].isnull();
    if (!object_has_state_flags)
    {
        DomainStateHistory history;
        typename DomainStateHistory::Record record;
        record.valid_from = static_cast<typename DomainStateHistory::TimePoint>(dbres[0][1]);
        history.timeline.push_back(record);
        history.valid_to = upper_limit;
        return history;
    }
    typename DomainStateHistory::Record record_on_begin;
    struct StateAction
    {
        DomainState state_begin;
        DomainState state_end;
    };
    using StateChanges = std::map<typename DomainStateHistory::TimePoint, StateAction>;
    StateChanges state_changes;
    for (std::size_t idx = 0; idx < dbres.size(); ++idx)
    {
        const bool collect_state_on_begin = static_cast<bool>(dbres[idx][3]);
        const auto valid_from = static_cast<typename DomainStateHistory::TimePoint>(dbres[idx][4]);
        const std::string flag_name = static_cast<std::string>(dbres[idx][6]);
        DomainState state;
        state.template visit<StateFlagSetter>(flag_name);
        if (collect_state_on_begin)
        {
            record_on_begin.state |= state;
            record_on_begin.valid_from = valid_from;
        }
        else
        {
            state_changes[valid_from].state_begin |= state;
        }
        if (!dbres[idx][5].isnull())
        {
            const auto valid_to = static_cast<typename DomainStateHistory::TimePoint>(dbres[idx][5]);
            if (valid_to <= upper_limit)
            {
                state_changes[valid_to].state_end |= state;
            }
        }
    }
    DomainStateHistory history;
    typename DomainStateHistory::Record record;
    if (record_on_begin.state.any())
    {
        record = record_on_begin;
        history.timeline.push_back(record);
    }
    else
    {
        const auto lower_limit = static_cast<typename DomainStateHistory::TimePoint>(dbres[0][1]);
        if (lower_limit < state_changes.begin()->first)
        {
            record.valid_from = lower_limit;
            history.timeline.push_back(record);
        }
    }
    for (const auto& time_action : state_changes)
    {
        record.valid_from = time_action.first;
        record.state |= time_action.second.state_begin;
        record.state &= ~time_action.second.state_end;
        history.timeline.push_back(record);
    }
    history.valid_to = upper_limit;
    return history;
}

}//namespace LibFred::RegistrableObject::Domain::{anonymous}

GetDomainStateHistoryById::GetDomainStateHistoryById(unsigned long long domain_id)
    : domain_id_(domain_id)
{
}

GetDomainStateHistoryById::Result GetDomainStateHistoryById::exec(
        OperationContext& ctx,
        const HistoryInterval& range) const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationById
    {
    public:
        explicit OperationById(unsigned long long object_id)
            : object_id_(object_id) { }
        std::string operator()(Database::query_param_list& params) const
        {
            return "$" + params.add(object_id_) + "::BIGINT";
        }
    private:
        const unsigned long long object_id_;
    };
    return get_domain_state_history(ctx, range, OperationById(domain_id_));
}

GetDomainStateHistoryByFqdn::GetDomainStateHistoryByFqdn(const std::string& _fqdn)
    : fqdn_(_fqdn)
{
}

GetDomainStateHistoryByFqdn::Result GetDomainStateHistoryByFqdn::exec(
        OperationContext& ctx,
        const HistoryInterval& range) const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationByFqdn
    {
    public:
        explicit OperationByFqdn(const std::string& _fqdn)
            : fqdn_(_fqdn) { }
        std::string operator()(Database::query_param_list& params) const
        {
            static const std::string sql_handle_case_normalize_function =
                    object_type == Object_Type::domain ? "LOWER"
                                                       : "UPPER";
            return "SELECT id "
                   "FROM object_registry "
                   "WHERE name=" + sql_handle_case_normalize_function + "($" + params.add(fqdn_) + "::TEXT) AND "
                         "type=get_object_type_id($" + params.add(Conversion::Enums::to_db_handle(object_type)) + "::TEXT) AND "
                         "erdate IS NULL";
        }
    private:
        const std::string fqdn_;
    };
    return get_domain_state_history(ctx, range, OperationByFqdn(fqdn_));
}

GetDomainStateHistoryByUuid::GetDomainStateHistoryByUuid(const DomainUuid& domain_uuid)
    : uuid_(domain_uuid)
{
}

GetDomainStateHistoryByUuid::Result GetDomainStateHistoryByUuid::exec(
        OperationContext& ctx,
        const HistoryInterval& range) const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationByUuid
    {
    public:
        explicit OperationByUuid(const DomainUuid& uuid)
            : uuid_(uuid) { }
        std::string operator()(Database::query_param_list& params) const
        {
            const auto object_type_param_text = "$" + params.add(Conversion::Enums::to_db_handle(object_type)) + "::TEXT";
            return "SELECT id "
                   "FROM object_registry "
                   "WHERE uuid=$" + params.add(uuid_) + "::UUID AND "
                         "type=get_object_type_id(" + object_type_param_text + ")";
        }
    private:
        const DomainUuid uuid_;
    };
    return get_domain_state_history(ctx, range, OperationByUuid(uuid_));
}

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred
