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
 *  @file
 *  object state check
 */

#include "libfred/object_state/object_has_state.hh"
#include "libfred/object_state/lock_object_state_request_lock.hh"

namespace LibFred {

ObjectHasState::ObjectHasState(unsigned long long object_id, Object_State::Enum state)
    : object_id_(object_id),
      state_(state)
{}

bool ObjectHasState::exec(const OperationContext& ctx)
{
    LockObjectStateRequestLock(object_id_).exec(ctx);

    const bool has_state = 0 < ctx.get_conn().exec_params(
            "SELECT 0 "
            "FROM object_state os "
            "JOIN enum_object_states eos ON eos.id=os.state_id "
            "WHERE os.object_id=$1::integer AND "
                  "eos.name=$2::text AND "
                  "os.valid_to IS NULL "
            "LIMIT 1",
            Database::query_param_list
                (object_id_)
                (Conversion::Enums::to_db_handle(state_))).size();
    return has_state;
}

}//namespace LibFred
