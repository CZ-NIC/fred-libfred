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
/**
 *  @file
 *  update contact check
 */
#include "libfred/registrable_object/contact/verification/update_check.hh"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign/list_of.hpp>

namespace LibFred {

UpdateContactCheck::UpdateContactCheck(
        const uuid& _check_handle,
        const std::string& _status_handle)
    : check_handle_(_check_handle),
      status_handle_(_status_handle)
{ }

UpdateContactCheck::UpdateContactCheck(
        const uuid& _check_handle,
        const std::string& _status_handle,
        const Optional<unsigned long long>& _logd_request_id)
    : check_handle_(_check_handle),
      status_handle_(_status_handle),
      logd_request_id_(_logd_request_id.isset() ? Nullable<unsigned long long>(_logd_request_id.get_value())
                                                : Nullable<unsigned long long>())
{ }

UpdateContactCheck& UpdateContactCheck::set_logd_request_id(unsigned long long _logd_request_id)
{
    logd_request_id_ = _logd_request_id;
    return *this;
}

void UpdateContactCheck::exec (OperationContext& _ctx)
{
    FREDLOG_DEBUG("UpdateContactCheck exec() started");
    FREDLOG_INFO(to_string());

    // using solo select for easy checking of existence (subselect would be strange)
    const Database::Result status_res = _ctx.get_conn().exec_params(
            "SELECT id "
            "FROM enum_contact_check_status "
            "WHERE handle=$1::varchar",
            Database::query_param_list(status_handle_));
    if (status_res.size() != 1)
    {
        throw ExceptionUnknownCheckStatusHandle();
    }
    const long status_id = static_cast<long>(status_res[0][0]);

    try
    {
        const Database::Result update_contact_check_res = _ctx.get_conn().exec_params(
                "UPDATE contact_check SET "
                    "(enum_contact_check_status_id,logd_request_id)="
                    "($1::int,$2::bigint) "
                "WHERE handle=$3::uuid "
                "RETURNING id",
                Database::query_param_list
                    (status_id)
                    (logd_request_id_)
                    (check_handle_));

        if (update_contact_check_res.size() != 1)
        {
            if (_ctx.get_conn().exec_params(
                        "SELECT 0 FROM contact_check WHERE handle=$1::uuid",
                        Database::query_param_list(check_handle_)).size() == 0)
            {
                throw ExceptionUnknownCheckHandle();
            }
            BOOST_THROW_EXCEPTION(LibFred::InternalError("contact_check update failed"));
        }
    }
    catch (const std::exception& e)
    {
        const std::string what_string = e.what();

        if (what_string.find("contact_check_fk_Enum_contact_check_status_id") != std::string::npos)
        {
            throw ExceptionUnknownCheckStatusHandle();
        }
        // problem was elsewhere so let it propagate
        throw;
    }

    FREDLOG_DEBUG("UpdateContactCheck executed successfully");
}

std::string UpdateContactCheck::to_string() const
{
    return Util::format_operation_state(
            "UpdateContactCheck",
            boost::assign::list_of
                (std::make_pair("check_handle", check_handle_.to_string()))
                (std::make_pair("logd_request_id", logd_request_id_.print_quoted()))
                (std::make_pair("status_handle", status_handle_)));
}

}//namespace LibFred
