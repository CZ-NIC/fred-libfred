/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
/**
 *  @file clear_object_state_request_id.cc
 *  clear object state request
 */

#include "libfred/object_state/clear_object_state_request_id.hh"
#include "libfred/object_state/lock_object_state_request_lock.hh"

namespace LibFred {

ClearObjectStateRequestId::ClearObjectStateRequestId(ObjectId _object_id)
    : object_id_(_object_id)
{}

ClearObjectStateRequestId::Requests ClearObjectStateRequestId::exec(OperationContext &_ctx)
{
    LockObjectStateRequestLock(object_id_).exec(_ctx);

    const Database::Result cmd_result = _ctx.get_conn().exec_params(
        "UPDATE object_state_request "
        "SET canceled=CURRENT_TIMESTAMP "
        "WHERE canceled is NULL AND "
              "object_id=$1::bigint AND "
              "valid_from<=CURRENT_TIMESTAMP AND "
              "(valid_to IS NULL OR "
               "CURRENT_TIMESTAMP<valid_to) AND "
              "(SELECT 0 "
               "FROM enum_object_states eos "
               "WHERE eos.id=state_id AND "
                     "eos.manual AND "
                     "eos.name LIKE 'server%' "
               "LIMIT 1) IS NOT NULL "
        "RETURNING id", Database::query_param_list(object_id_));
    Requests result;
    if (0 < cmd_result.size())
    {
        std::string rid = "ClearObjectStateRequest::exec canceled request id:";
        for (std::size_t idx = 0; idx < cmd_result.size(); ++idx)
        {
            rid += " " + static_cast<std::string>(cmd_result[idx][0]);
            result.push_back(static_cast<LibFred::ObjectId>(cmd_result[idx][0]));
        }
        FREDLOG_DEBUG(rid);
    }
    return result;
}

}//namespace LibFred
