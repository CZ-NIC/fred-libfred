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
#ifndef GET_HANDLE_HISTORY_IMPL_HH_8CB5C35DF83800C9F3754CA875C4BCE4//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_HANDLE_HISTORY_IMPL_HH_8CB5C35DF83800C9F3754CA875C4BCE4

#include "libfred/registrable_object/get_handle_history.hh"
#include "libfred/registrable_object/exceptions_impl.hh"

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
GetHandleHistory<o>::GetHandleHistory(const std::string& handle)
    : handle_(handle)
{ }

namespace {

template <Object_Type::Enum o>
constexpr char sql_get_handle_history[] =
        "SELECT obr.uuid,obr.crdate,bh.uuid,obr.erdate,eh.uuid "
        "FROM object_registry obr "
        "JOIN history bh ON bh.id=obr.crhistoryid "
        "JOIN history eh ON eh.id=obr.historyid "
        "WHERE obr.type=get_object_type_id($1::TEXT) AND "
              "UPPER(obr.name)=UPPER($2::TEXT) "
        "ORDER BY obr.crdate";

template <>
constexpr char sql_get_handle_history<Object_Type::domain>[] =
        "SELECT obr.uuid,obr.crdate,bh.uuid,obr.erdate,eh.uuid "
        "FROM object_registry obr "
        "JOIN history bh ON bh.id=obr.crhistoryid "
        "JOIN history eh ON eh.id=obr.historyid "
        "WHERE obr.type=get_object_type_id($1::TEXT) AND "
              "obr.name=LOWER($2::TEXT) "
        "ORDER BY obr.crdate";

}//namespace LibFred::RegistrableObject::{anonymous}

template <Object_Type::Enum o>
typename GetHandleHistory<o>::Result GetHandleHistory<o>::exec(OperationContext& ctx)const
{
    static const std::string sql = sql_get_handle_history<object_type>;
    const Database::QueryParams params =
            {
                Conversion::Enums::to_db_handle(object_type),
                handle_
            };
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    Result history;
    history.handle = handle_;
    for (unsigned long long idx = 0; idx < dbres.size(); ++idx)
    {
        typename Result::Record record;
        record.uuid = dbres[idx][0].as<UuidOf<o>>();
        record.begin.timestamp = static_cast<typename Result::TimePoint>(dbres[idx][1]);
        record.begin.history_uuid = dbres[idx][2].as<HistoryUuidOf<o>>();
        if (!dbres[idx][3].isnull())
        {
            record.end.timestamp = static_cast<typename Result::TimePoint>(dbres[idx][3]);
        }
        record.end.history_uuid = dbres[idx][4].as<HistoryUuidOf<o>>();
        history.timeline.push_back(record);
    }
    return history;
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_HANDLE_HISTORY_IMPL_HH_8CB5C35DF83800C9F3754CA875C4BCE4
