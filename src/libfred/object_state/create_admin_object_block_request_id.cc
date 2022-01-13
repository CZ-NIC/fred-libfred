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
 *  @file create_admin_object_block_request_id.cc
 *  create administrative object block request
 */

#include "libfred/object_state/create_admin_object_block_request_id.hh"
#include "libfred/object_state/clear_object_state_request_id.hh"
#include "libfred/object_state/get_blocking_status_desc_list.hh"
#include "libfred/object_state/lock_object_state_request_lock.hh"

#include <boost/algorithm/string.hpp>
#include <set>

namespace LibFred {

CreateAdminObjectBlockRequestId::CreateAdminObjectBlockRequestId(
        ObjectId _object_id,
        const StatusList& _status_list)
    : object_id_(_object_id),
      status_list_(_status_list)
{}

CreateAdminObjectBlockRequestId::CreateAdminObjectBlockRequestId(
        ObjectId _object_id,
        const StatusList& _status_list,
        const Optional< Time >& _valid_from,
        const Optional< Time >& _valid_to,
        const std::string& _reason,
        const Optional<unsigned long long>& _logd_request_id)
    : object_id_(_object_id),
      status_list_(_status_list),
      valid_from_(_valid_from),
      valid_to_(_valid_to),
      reason_(_reason),
      logd_request_id_(_logd_request_id.isset()
                       ? Nullable<unsigned long long>(_logd_request_id.get_value())
                       : Nullable<unsigned long long>())
{}

CreateAdminObjectBlockRequestId& CreateAdminObjectBlockRequestId::set_valid_from(const Time& _valid_from)
{
    valid_from_ = _valid_from;
    return *this;
}

CreateAdminObjectBlockRequestId& CreateAdminObjectBlockRequestId::set_valid_to(const Time& _valid_to)
{
    valid_to_ = _valid_to;
    return *this;
}

CreateAdminObjectBlockRequestId& CreateAdminObjectBlockRequestId::set_reason(const std::string& _reason)
{
    reason_ = _reason;
    return *this;
}

CreateAdminObjectBlockRequestId& CreateAdminObjectBlockRequestId::set_logd_request_id(unsigned long long _logd_request_id)
{
    logd_request_id_ = _logd_request_id;
    return *this;
}

std::string CreateAdminObjectBlockRequestId::exec(const OperationContext& _ctx)
{
    this->check_administrative_block_status_only(_ctx);
    this->check_server_blocked_status_absent(_ctx);
    ClearObjectStateRequestId(object_id_).exec(_ctx);
    StatusList status_list = status_list_;
    status_list.insert("serverBlocked");
    CreateObjectStateRequestId createObjectStateRequestId(
            object_id_,
            status_list,
            valid_from_,
            valid_to_);
    const std::string handle_name = createObjectStateRequestId.exec(_ctx).first;
    if (reason_.isset())
    {
        _ctx.get_conn().exec_params(
            "INSERT INTO object_state_request_reason (object_state_request_id,reason_creation,reason_cancellation) "
                "SELECT osr.id,$1::text,NULL "
                "FROM object_state_request osr "
                "JOIN enum_object_states eos ON eos.id=osr.state_id "
                "WHERE osr.valid_from<=CURRENT_TIMESTAMP AND "
                      "(osr.valid_to IS NULL OR "
                       "CURRENT_TIMESTAMP<=osr.valid_to) AND "
                      "osr.canceled IS NULL AND "
                      "osr.object_id=$2::integer AND "
                      "eos.name='serverBlocked' "
                "ORDER BY osr.id DESC LIMIT 1",
            Database::query_param_list(reason_.get_value())
                                      (object_id_));
    }
    else
    {
        _ctx.get_conn().exec_params(
            "INSERT INTO object_state_request_reason (object_state_request_id,reason_creation,reason_cancellation) "
                "SELECT osr.id,NULL,NULL "
                "FROM object_state_request osr "
                "JOIN enum_object_states eos ON eos.id=osr.state_id "
                "WHERE osr.valid_from<=CURRENT_TIMESTAMP AND "
                      "(osr.valid_to IS NULL OR "
                       "CURRENT_TIMESTAMP<=osr.valid_to) AND "
                      "osr.canceled IS NULL AND "
                      "osr.object_id=$1::integer AND "
                      "eos.name='serverBlocked' "
                "ORDER BY osr.id DESC LIMIT 1",
            Database::query_param_list(object_id_));
    }
    return handle_name;
}

void CreateAdminObjectBlockRequestId::check_administrative_block_status_only(const OperationContext& _ctx)const
{
    if (status_list_.empty())
    {
        BOOST_THROW_EXCEPTION(Exception().set_vector_of_state_not_found(std::vector< std::string >()));
    }
    typedef std::set<std::string> StatusSet;
    typedef GetBlockingStatusDescList::StatusDescList StatusDescList;
    static StatusSet administrativeBlockStatusSet; // set of administrative block status
    if (administrativeBlockStatusSet.empty())
    {
        GetBlockingStatusDescList getBlockingStatusDescList;
        const StatusDescList &statusDescList = getBlockingStatusDescList.exec(_ctx);
        for (StatusDescList::const_iterator pItem = statusDescList.begin(); pItem != statusDescList.end(); ++pItem)
        {
            administrativeBlockStatusSet.insert(pItem->status);
        }
    }
    Exception ex;
    for (StatusList::const_iterator pState = status_list_.begin(); pState != status_list_.end(); ++pState)
    {
        if (administrativeBlockStatusSet.count(*pState) <= 0)
        {
            ex.add_state_not_found(*pState);
        }
    }
    if (ex.throw_me())
    {
        BOOST_THROW_EXCEPTION(ex);
    }
}

void CreateAdminObjectBlockRequestId::check_server_blocked_status_absent(const OperationContext& _ctx)const
{
    static unsigned long long serverBlockedId = 0;
    if (serverBlockedId == 0)
    {
        const Database::Result obj_state_res = _ctx.get_conn().exec(
            "SELECT id "
            "FROM enum_object_states "
            "WHERE name='serverBlocked'");

        if (obj_state_res.size() != 1)
        {
            BOOST_THROW_EXCEPTION(Exception().add_state_not_found("serverBlocked"));
        }
        serverBlockedId = static_cast<unsigned long long>(obj_state_res[0][0]);
        FREDLOG_DEBUG("serverBlockedId = " + boost::lexical_cast<std::string>(serverBlockedId));
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
            (serverBlockedId));
    if (rcheck.size() <= 0)
    {
        return;
    }
    BOOST_THROW_EXCEPTION(Exception().set_server_blocked_present(object_id_));
}

}//namespace LibFred
