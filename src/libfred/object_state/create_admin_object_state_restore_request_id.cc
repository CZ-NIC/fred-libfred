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
 *  @file create_admin_object_state_restore_request_id.cc
 *  create administrative object state restore request
 */

#include "libfred/object_state/create_admin_object_state_restore_request_id.hh"
#include "libfred/object_state/clear_admin_object_state_request_id.hh"
#include "libfred/object_state/clear_object_state_request_id.hh"
#include "libfred/object_state/create_object_state_request_id.hh"
#include "libfred/object_state/get_blocking_status_desc_list.hh"
#include "libfred/object_state/lock_object_state_request_lock.hh"

#include "libfred/types.hh"

#include <boost/algorithm/string.hpp>

namespace LibFred {

CreateAdminObjectStateRestoreRequestId::CreateAdminObjectStateRestoreRequestId(ObjectId _object_id)
    : object_id_(_object_id)
{}

CreateAdminObjectStateRestoreRequestId::CreateAdminObjectStateRestoreRequestId(
        ObjectId _object_id,
        const std::string& _reason,
        const Optional<unsigned long long>& _logd_request_id)
    : object_id_(_object_id),
      reason_(_reason),
      logd_request_id_(_logd_request_id.isset()
                       ? Nullable<unsigned long long>(_logd_request_id.get_value())
                       : Nullable<unsigned long long>())
{}

CreateAdminObjectStateRestoreRequestId& CreateAdminObjectStateRestoreRequestId::set_reason(
        const std::string& _reason)
{
    reason_ = _reason;
    return *this;
}

CreateAdminObjectStateRestoreRequestId& CreateAdminObjectStateRestoreRequestId::set_logd_request_id(
        unsigned long long _logd_request_id)
{
    logd_request_id_ = _logd_request_id;
    return *this;
}

void CreateAdminObjectStateRestoreRequestId::exec(const OperationContext& _ctx)
{
    const ObjectStateId server_blocked_id = this->check_server_blocked_status_present(_ctx);
    enum ResultColumnIndex
    {
        OBJECT_STATE_ID_IDX = 0,
        CURRENT_IDX         = 1,
        FOLLOW_UP_IDX       = 2,
    };
    const Database::Result block_history = _ctx.get_conn().exec_params(
        "SELECT os.id," // STATE_ID
               "os.valid_to IS NULL OR CURRENT_TIMESTAMP<=os.valid_to," // CURRENT
               "COALESCE((SELECT valid_to "
                "FROM object_state "
                "WHERE object_id=$1::bigint AND state_id=$2::bigint AND valid_from<os.valid_from "
                "ORDER BY valid_from DESC LIMIT 1"
               ")=os.valid_from,false) " // FOLLOW_UP
        "FROM object_state os "
        "WHERE os.object_id=$1::bigint AND os.state_id=$2::bigint ORDER BY os.valid_from DESC",
        Database::query_param_list(object_id_)(server_blocked_id));
    Database::Result::Iterator pRow = block_history.begin();
    // seek on actual serverBlocked
    while (pRow != block_history.end())
    {
        if (static_cast<bool>((*pRow)[CURRENT_IDX]))
        {
            break;
        }
        ++pRow;
    }
    // seek on begin of serverBlocked sequence
    while (pRow != block_history.end())
    {
        if (!static_cast<bool>((*pRow)[FOLLOW_UP_IDX]))
        {
            break;
        }
        ++pRow;
    }
    StatusList previous_status_list;
    if (pRow != block_history.end())
    {

        LockObjectStateRequestLock(object_id_).exec(_ctx);

        const TID start_object_state_id = (*pRow)[OBJECT_STATE_ID_IDX];
        const Database::Result previous_status_list_result = _ctx.get_conn().exec_params(
            "SELECT eos.name "
            "FROM (SELECT object_id,valid_from FROM object_state WHERE id=$1::integer) oss "
            "JOIN object_state os ON (os.object_id=oss.object_id AND "
                                     "os.valid_from<oss.valid_from AND "
                                     "(os.valid_to IS NULL OR oss.valid_from<=os.valid_to)) "
            "JOIN enum_object_states eos ON eos.id=os.state_id "
            "WHERE eos.manual AND name LIKE 'server%'",
            Database::query_param_list(start_object_state_id));
        for (Database::Result::Iterator pName = previous_status_list_result.begin();
             pName != previous_status_list_result.end(); ++pName)
        {
            previous_status_list.insert((*pName)[0]);
        }
    }
    try
    {
        ClearAdminObjectStateRequestId(object_id_, reason_.get_value()).exec(_ctx);
    }
    catch (const ClearAdminObjectStateRequestId::Exception &ex)
    {
        if (ex.is_set_server_blocked_absent())
        {
            BOOST_THROW_EXCEPTION(Exception().set_server_blocked_absent(ex.get_server_blocked_absent()));
        }
        throw;
    }
    if (!previous_status_list.empty())
    {
        CreateObjectStateRequestId(object_id_, previous_status_list).exec(_ctx);
    }
}

ObjectStateId CreateAdminObjectStateRestoreRequestId::check_server_blocked_status_present(
        const OperationContext& _ctx)const
{
    static ObjectStateId server_blocked_id = 0;
    if (server_blocked_id == 0)
    {
        const Database::Result obj_state_res = _ctx.get_conn().exec(
            "SELECT id "
            "FROM enum_object_states "
            "WHERE name='serverBlocked'");

        if (obj_state_res.size() != 1)
        {
            BOOST_THROW_EXCEPTION(Exception().set_state_not_found("serverBlocked"));
        }
        server_blocked_id = static_cast<ObjectStateId>(obj_state_res[0][0]);
        FREDLOG_DEBUG("serverBlockedId = " + boost::lexical_cast< std::string >(server_blocked_id));
    }
    FREDLOG_DEBUG("LockObjectStateRequestLock call");
    LockObjectStateRequestLock(object_id_).exec(_ctx);
    FREDLOG_DEBUG("LockObjectStateRequestLock success");
    const Database::Result rcheck = _ctx.get_conn().exec_params(
        "SELECT 1 "
        "FROM object_state "
        "WHERE object_id=$1::integer AND "
              "state_id=$2::integer AND "
              "valid_from<=CURRENT_TIMESTAMP AND "
              "(valid_to IS NULL OR "
               "CURRENT_TIMESTAMP<valid_to) "
              "LIMIT 1",
        Database::query_param_list
            (object_id_)
            (server_blocked_id));
    if (0 < rcheck.size())
    {
        return server_blocked_id;
    }
    BOOST_THROW_EXCEPTION(Exception().set_server_blocked_absent(object_id_));
}

}//namespace LibFred
