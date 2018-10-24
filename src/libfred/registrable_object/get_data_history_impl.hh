/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GET_DATA_HISTORY_IMPL_HH_F254A142947B7FDDBB8239A8795B3DCC//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_DATA_HISTORY_IMPL_HH_F254A142947B7FDDBB8239A8795B3DCC

#include "libfred/registrable_object/get_data_history.hh"
#include "libfred/registrable_object/get_state_history_impl.hh"
#include "libfred/registrable_object/exceptions_impl.hh"

namespace LibFred {
namespace RegistrableObject {

template <typename D, typename H>
typename GetDataHistory<D, H>::Result GetDataHistory<D, H>::exec(
        OperationContext& ctx,
        const HistoryInterval& range)const
{
    class LowerLimitVisitor : public boost::static_visitor<std::string>
    {
    public:
        LowerLimitVisitor(Database::query_param_list& params) : params_(params) { }
        std::string operator()(const HistoryInterval::NoLimit&)const
        {
            return "crdate";
        }
        std::string operator()(const HistoryInterval::HistoryId& history_id)const
        {
            return "SELECT valid_from FROM history WHERE id=$" + params_.add(history_id.value) + "::BIGINT";
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
        std::string operator()(const HistoryInterval::HistoryId& history_id)const
        {
            return "SELECT valid_from FROM history WHERE id=$" + params_.add(history_id.value) + "::BIGINT";
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
    const std::string object_id_rule = this->derived().get_object_id_rule(params);
    const auto lower_limit_rule = boost::apply_visitor(LowerLimitVisitor(params), range.lower_limit);
    const auto upper_limit_rule = boost::apply_visitor(UpperLimitVisitor(params), range.upper_limit);
    const std::string sql =
            "WITH o AS ("
                "SELECT id,"
                       "(" + lower_limit_rule + ") AS lower_limit,"
                       "(" + upper_limit_rule + ") AS upper_limit "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=(" + object_id_rule + ")) "
            "SELECT o.id,h.id,h.valid_from,h.valid_to,h.request_id "
            "FROM o "
            "JOIN object_history oh ON oh.id=o.id "
            "JOIN history h ON h.id=oh.historyid AND "
                              "(o.lower_limit<=h.valid_to OR h.valid_to IS NULL) AND "
                              "h.valid_from<=o.upper_limit "
            "ORDER BY h.valid_from";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        if (ctx.get_conn().exec_params(
                "SELECT 0 "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=(" + object_id_rule + ")", params).size() <= 0)
        {
            ctx.get_log().debug(Conversion::Enums::to_db_handle(object_type) + " does not exist");
            throw DoesNotExist();
        }
        ctx.get_log().debug("invalid " + Conversion::Enums::to_db_handle(object_type) + " history interval");
        throw InvalidHistoryIntervalSpecification();
    }
    Result history;
    for (std::size_t idx = 0; idx < dbres.size(); ++idx)
    {
        if (idx == 0)
        {
            history.object_id = static_cast<unsigned long long>(dbres[0][0]);
        }
        typename Result::Record record;
        record.history_id = static_cast<unsigned long long>(dbres[idx][1]);
        record.valid_from = static_cast<typename Result::TimePoint>(dbres[idx][2]);
        if (dbres[idx][3].isnull())
        {
            history.valid_to = boost::none;
        }
        else
        {
            history.valid_to = static_cast<typename Result::TimePoint>(dbres[idx][3]);
        }
        if (!dbres[idx][4].isnull())
        {
            record.log_request_id = static_cast<unsigned long long>(dbres[idx][4]);
        }
        history.timeline.push_back(record);
    }
    return history;
}

template <typename D, typename S>
const D& GetDataHistory<D, S>::derived()const
{
    return *static_cast<const D*>(this);
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_DATA_HISTORY_IMPL_HH_F254A142947B7FDDBB8239A8795B3DCC
