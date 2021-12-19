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
#include "libfred/public_request/get_opened_public_request.hh"
#include "libfred/public_request/update_public_request.hh"
#include "libfred/public_request/public_request_lock_guard.hh"
#include "libfred/public_request/public_request_status.hh"

namespace LibFred {

GetOpenedPublicRequest::GetOpenedPublicRequest(const PublicRequestTypeIface& _type)
:   type_(_type.get_public_request_type())
{ }

PublicRequestId GetOpenedPublicRequest::exec(
    const OperationContext& _ctx,
    const LockedPublicRequestsOfObject& _locked_object,
    const Optional<LogRequestId>&)const
{
    Database::query_param_list params(type_);// $1::TEXT
    params(_locked_object.get_id());         // $2::BIGINT
    const Database::Result res = _ctx.get_conn().exec_params(
        "SELECT (SELECT id FROM public_request "
                "WHERE request_type=enum_public_request_type.id AND resolve_time IS NULL AND "
                      "EXISTS(SELECT 1 FROM public_request_objects_map "
                             "WHERE request_id=public_request.id AND object_id=$2::BIGINT) "
                "LIMIT 1) "
        "FROM enum_public_request_type "
        "WHERE name=$1::TEXT", params);
    if (res.size() <= 0)
    {
        BOOST_THROW_EXCEPTION(Exception().set_unknown_type(type_));
    }
    if (res[0][0].isnull())
    {
        BOOST_THROW_EXCEPTION(Exception().set_no_request_found(type_));
    }
    const PublicRequestId found_public_request_id = static_cast< PublicRequestId >(res[0][0]);
    return found_public_request_id;
}

} // namespace LibFred
