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
#include "libfred/public_request/public_request_lock_guard.hh"

namespace LibFred
{

namespace
{

PublicRequestId lock_by_identification(const OperationContext& _ctx, const std::string &_identification);
PublicRequestId lock_by_id(const OperationContext& _ctx, PublicRequestId _id);

}

PublicRequestLockGuardByIdentification::PublicRequestLockGuardByIdentification(
    const OperationContext& _ctx,
    const std::string &_identification)
:   ctx_(_ctx),
    public_request_id_(lock_by_identification(_ctx, _identification))
{
}

PublicRequestLockGuardById::PublicRequestLockGuardById(
    const OperationContext& _ctx,
    PublicRequestId _id)
:   ctx_(_ctx),
    public_request_id_(lock_by_id(_ctx, _id))
{
}

namespace
{

PublicRequestId lock_by_identification(const OperationContext& _ctx, const std::string &_identification)
{
    //get lock to the end of transaction for given object
    const Database::Result res = _ctx.get_conn().exec_params(
        "SELECT prom.request_id,lock_public_request_lock(prom.object_id) "
        "FROM public_request_auth pra "
        "JOIN public_request_objects_map prom ON prom.request_id=pra.id "
        "WHERE pra.identification=$1::TEXT",
        Database::query_param_list(_identification));
    if (0 < res.size()) {
        const PublicRequestId public_request_id = static_cast< PublicRequestId >(res[0][0]);
        return public_request_id;
    }
    BOOST_THROW_EXCEPTION(PublicRequestLockGuardByIdentification::Exception().
                              set_public_request_doesnt_exist(_identification));
}

PublicRequestId lock_by_id(const OperationContext& _ctx, PublicRequestId _id)
{
    //get lock to the end of transaction for given object
    const Database::Result res = _ctx.get_conn().exec_params(
        "SELECT lock_public_request_lock(object_id) "
        "FROM public_request_objects_map "
        "WHERE request_id=$1::BIGINT",
        Database::query_param_list(_id));
    if (0 < res.size()) {
        return _id;
    }
    BOOST_THROW_EXCEPTION(PublicRequestLockGuardById::Exception().
                              set_public_request_doesnt_exist(_id));
}

}

} // namespace LibFred
