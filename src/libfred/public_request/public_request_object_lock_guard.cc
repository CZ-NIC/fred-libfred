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
#include "libfred/public_request/public_request_object_lock_guard.hh"

namespace LibFred {

PublicRequestsOfObjectLockGuardByObjectId::PublicRequestsOfObjectLockGuardByObjectId(
        OperationContext &_ctx,
        ObjectId _object_id)
:   ctx_(_ctx),
    object_id_(_object_id)
{
    //get lock to the end of transaction for given object
    if (0 < _ctx.get_conn().exec_params("SELECT lock_public_request_lock(id) "
                                        "FROM object "
                                        "WHERE id=$1::BIGINT",
                                        Database::query_param_list(_object_id)).size()) {
        return;
    }
    BOOST_THROW_EXCEPTION(Exception().set_object_doesnt_exist(_object_id));
}

} // namespace LibFred
