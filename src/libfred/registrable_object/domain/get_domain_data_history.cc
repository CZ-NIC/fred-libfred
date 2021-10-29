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
#include "libfred/registrable_object/domain/get_domain_data_history.hh"

#include "libfred/registrable_object/history_interval_impl.hh"
#include "libfred/registrable_object/exceptions_impl.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

namespace {

template <Object_Type::Enum object_type, typename T>
bool does_object_exist(
        OperationContext& ctx,
        T get_object_id_rule)
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    const std::string object_id_rule = get_object_id_rule(params);
    return 0 < ctx.get_conn().exec_params(
            "SELECT 0 "
            "FROM object_registry "
            "WHERE type=get_object_type_id($1::TEXT) AND "
                  "id=(" + object_id_rule + ")", params).size();
}

template <typename T>
DomainDataHistory get_domain_data_history(
        OperationContext& ctx,
        const HistoryInterval& range,
        T get_object_id_rule)
{
    static constexpr auto object_type = Object_Type::domain;
    class LowerLimitVisitor : public boost::static_visitor<std::string>
    {
    public:
        LowerLimitVisitor(Database::query_param_list& params) : params_(params) { }
        std::string operator()(const HistoryInterval::NoLimit&)const
        {
            return "crdate";
        }
        std::string operator()(const ObjectHistoryUuid& history_uuid)const
        {
            return "SELECT valid_from FROM history WHERE uuid=$" + params_.add(history_uuid) + "::UUID";
        }
        std::string operator()(const HistoryInterval::TimePoint<std::chrono::nanoseconds>& at)const
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
        std::string operator()(const HistoryInterval::NoLimit&)const
        {
            return "'infinity'::TIMESTAMP";
        }
        std::string operator()(const ObjectHistoryUuid& history_uuid)const
        {
            return "SELECT valid_from FROM history WHERE uuid=$" + params_.add(history_uuid) + "::UUID";
        }
        std::string operator()(const HistoryInterval::TimePoint<std::chrono::nanoseconds>& at)const
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
                "SELECT id,uuid,"
                       "(" + lower_limit_rule + ") AS lower_limit,"
                       "(" + upper_limit_rule + ") AS upper_limit "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=(" + object_id_rule + ")) "
            "SELECT o.uuid,h.uuid,h.valid_from,h.valid_to,h.request_id "
            "FROM o "
            "JOIN object_history oh ON oh.id=o.id "
            "JOIN history h ON h.id=oh.historyid AND "
                              "(o.lower_limit<=h.valid_to OR h.valid_to IS NULL) AND "
                              "h.valid_from<=o.upper_limit "
            "ORDER BY h.valid_from";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        if (!does_object_exist<object_type>(ctx, get_object_id_rule))
        {
            FREDLOG_DEBUG(Conversion::Enums::to_db_handle(object_type) + " does not exist");
            throw ObjectDoesNotExist<object_type>();
        }
        FREDLOG_DEBUG("invalid " + Conversion::Enums::to_db_handle(object_type) + " history interval");
        throw InvalidHistoryIntervalSpecification<object_type>();
    }
    DomainDataHistory history;
    for (std::size_t idx = 0; idx < dbres.size(); ++idx)
    {
        if (idx == 0)
        {
            history.object_uuid = dbres[0][0].as<DomainUuid>();
        }
        typename DomainDataHistory::Record record;
        record.history_uuid = dbres[idx][1].as<DomainHistoryUuid>();
        record.valid_from = static_cast<typename DomainDataHistory::TimePoint>(dbres[idx][2]);
        if (dbres[idx][3].isnull())
        {
            history.valid_to = boost::none;
        }
        else
        {
            history.valid_to = static_cast<typename DomainDataHistory::TimePoint>(dbres[idx][3]);
        }
        if (!dbres[idx][4].isnull())
        {
            record.log_request_id = static_cast<unsigned long long>(dbres[idx][4]);
        }
        history.timeline.push_back(record);
    }
    return history;
}

}//namespace LibFred::RegistrableObject::Domain::{anonymous}

GetDomainDataHistoryById::GetDomainDataHistoryById(unsigned long long domain_id)
    : domain_id_(domain_id)
{ }

GetDomainDataHistoryById::Result GetDomainDataHistoryById::exec(
        OperationContext& ctx,
        const HistoryInterval& range)const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationById
    {
    public:
        explicit OperationById(unsigned long long object_id)
            : object_id_(object_id) { }
        std::string operator()(Database::query_param_list& params)const
        {
            return "$" + params.add(object_id_) + "::BIGINT";
        }
    private:
        const unsigned long long object_id_;
    };
    return get_domain_data_history(ctx, range, OperationById(domain_id_));
}

GetDomainDataHistoryByFqdn::GetDomainDataHistoryByFqdn(const std::string& fqdn)
    : handle_(fqdn)
{
}

GetDomainDataHistoryByFqdn::Result GetDomainDataHistoryByFqdn::exec(
        OperationContext& ctx,
        const HistoryInterval& range)const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationByFqdn
    {
    public:
        explicit OperationByFqdn(const std::string& handle)
            : handle_(handle) { }
        std::string operator()(Database::query_param_list& params)const
        {
            static const std::string sql_handle_case_normalize_function =
                    object_type == Object_Type::domain ? "LOWER"
                                                       : "UPPER";
            return "SELECT id "
                   "FROM object_registry "
                   "WHERE name=" + sql_handle_case_normalize_function + "($" + params.add(handle_) + "::TEXT) AND "
                         "type=get_object_type_id($" + params.add(Conversion::Enums::to_db_handle(object_type)) + "::TEXT) AND "
                         "erdate IS NULL";
        }
    private:
        const std::string handle_;
    };
    return get_domain_data_history(ctx, range, OperationByFqdn(handle_));
}

GetDomainDataHistoryByUuid::GetDomainDataHistoryByUuid(const DomainUuid& domain_uuid)
    : uuid_(domain_uuid)
{ }

GetDomainDataHistoryByUuid::Result GetDomainDataHistoryByUuid::exec(
        OperationContext& ctx,
        const HistoryInterval& range)const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    class OperationByUuid
    {
    public:
        explicit OperationByUuid(const DomainUuid& uuid)
            : uuid_(uuid) { }
        std::string operator()(Database::query_param_list& params)const
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
    return get_domain_data_history(ctx, range, OperationByUuid(uuid_));
}

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred
