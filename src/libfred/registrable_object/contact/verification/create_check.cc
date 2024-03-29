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
 *  create contact check
 */

#include "libfred/registrable_object/contact/verification/create_check.hh"
#include "libfred/registrable_object/contact/verification/create_test.hh"
#include "libfred/registrable_object/contact/verification/enum_check_status.hh"

#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include <utility>

namespace LibFred {

CreateContactCheck::CreateContactCheck(
        unsigned long long _contact_id,
        const std::string& _testsuite_handle)
    : contact_id_(_contact_id),
      testsuite_handle_(_testsuite_handle)
{ }

CreateContactCheck::CreateContactCheck(
        unsigned long long _contact_id,
        const std::string& _testsuite_handle,
        const Optional<unsigned long long>& _logd_request_id)
    : contact_id_(_contact_id),
      testsuite_handle_(_testsuite_handle),
      logd_request_id_(_logd_request_id.isset() ? Nullable<unsigned long long>(_logd_request_id.get_value())
                                                : Nullable<unsigned long long>())
{ }

CreateContactCheck& CreateContactCheck::set_logd_request_id(unsigned long long _logd_request_id)
{
    logd_request_id_ = _logd_request_id;
    return *this;
}

std::string CreateContactCheck::exec(const OperationContext& _ctx)
{
    FREDLOG_DEBUG("CreateContactCheck exec() started");
    FREDLOG_INFO(to_string());

    LibFred::OperationContextCreator ctx_unique;
    const std::string unique_test_query =
            "SELECT handle "
            "FROM contact_check "
            "WHERE handle=$1::uuid";
    // generate handle over and over until it is unique
    std::string handle;
    do
    {
        handle = boost::lexical_cast<std::string>(boost::uuids::random_generator()());
    } while (ctx_unique.get_conn().exec_params(unique_test_query,
                                               Database::query_param_list(handle)).size() != 0);

    // using solo select for easy checking of existence (subselect would be strange)
    const Database::Result contact_history_res = _ctx.get_conn().exec_params(
            "SELECT obj_reg.historyid "
            "FROM object_registry AS obj_reg "
            "JOIN enum_object_type AS e_o_t ON obj_reg.type=e_o_t.id "
            "WHERE obj_reg.id=$1::integer AND "
                  "e_o_t.name=$2::varchar AND "
                  "obj_reg.erdate IS NULL",
            Database::query_param_list
                (contact_id_)
                ("contact"));
    if (contact_history_res.size() != 1)
    {
        throw ExceptionUnknownContactId();
    }
    const unsigned long long contact_history_id = static_cast<unsigned long long>(contact_history_res[0][0]);

    const Database::Result testsuite_res = _ctx.get_conn().exec_params(
            "SELECT id "
            "FROM enum_contact_testsuite "
            "WHERE handle=$1::varchar",
            Database::query_param_list(testsuite_handle_));
    if (testsuite_res.size() != 1)
    {
        throw ExceptionUnknownTestsuiteHandle();
    }
    const long testsuite_id = static_cast<long>(testsuite_res[0]["id"]);

    try
    {
        _ctx.get_conn().exec_params(
            "INSERT INTO contact_check ("
                "handle,"
                "contact_history_id,"
                "enum_contact_testsuite_id,"
                "enum_contact_check_status_id,"
                "logd_request_id) "
            "VALUES ("
                "$1::uuid,"
                "$2::int,"
                "$3::int,"
                "(SELECT id FROM enum_contact_check_status WHERE handle=$4::varchar),"
                "$5::bigint)",
            Database::query_param_list
                (handle)
                (contact_history_id)
                (testsuite_id)
                (LibFred::ContactCheckStatus::ENQUEUE_REQ)
                (logd_request_id_));
    }
    catch (const std::exception& e)
    {
        const std::string what_string = e.what();

        if (what_string.find("fk_contact_check_contact_history_id") != std::string::npos)
        {
            throw ExceptionUnknownContactId();
        }

        if (what_string.find("contact_check_fk_Enum_contact_testsuite_id") != std::string::npos)
        {
            throw ExceptionUnknownTestsuiteHandle();
        }

        // problem was elsewhere so let it propagate
        throw;
    }

    FREDLOG_DEBUG("CreateContactCheck executed successfully");

    return handle;
}

std::string CreateContactCheck::to_string()const
{
    return Util::format_operation_state(
            "CreateContactCheck",
            boost::assign::list_of
                (std::make_pair("contact_id", boost::lexical_cast<std::string>(contact_id_)))
                (std::make_pair("testsuite_handle", testsuite_handle_))
                (std::make_pair("logd_request_id", logd_request_id_.print_quoted())));
}

}//namespace LibFred
