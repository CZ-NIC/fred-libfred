/*
 * Copyright (C) 2013  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file
 *  (get) list (of) contact check
 */

#include "libfred/registrable_object/contact/verification/list_checks.hh"
#include "libfred/registrable_object/contact/verification/enum_check_status.hh"
#include "libfred/opexception.hh"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign/list_of.hpp>

#include <map>
#include <utility>

namespace LibFred {

std::string ListChecksItem::to_string(const std::string& _each_line_prefix) const
{
    return _each_line_prefix + "<ListChecksItem> {" + "\n" +
           _each_line_prefix + _each_line_prefix + " check_handle: " + check_handle + "\n" +
           _each_line_prefix + _each_line_prefix + " testsuite_handle: " + testsuite_handle + "\n" +
           _each_line_prefix + _each_line_prefix + " contact_history_id: " +
                   boost::lexical_cast<std::string>(contact_history_id) + "\n" +
           _each_line_prefix + _each_line_prefix + " local_create_time: " +
                   boost::posix_time::to_simple_string(local_create_time) + "\n" +
           _each_line_prefix + _each_line_prefix + " status_handle: " + status_handle + "\n" +
           _each_line_prefix + "}\n";
}

ListContactChecks::ListContactChecks(
        const Optional<unsigned long>& _max_item_count,
        const Optional<std::string>& _testsuite_handle,
        const Optional<unsigned long long>& _contact_id,
        const Optional<std::string>& _status_handle)
    : max_item_count_(_max_item_count),
      testsuite_handle_(_testsuite_handle),
      contact_id_(_contact_id),
      status_handle_(_status_handle)
{ }

ListContactChecks& ListContactChecks::set_max_item_count(unsigned long _max_item_count)
{
    max_item_count_ = _max_item_count;
    return *this;
}

ListContactChecks& ListContactChecks::set_testsuite_handle(const std::string& _testsuite_handle)
{
    testsuite_handle_ = _testsuite_handle;
    return *this;
}

ListContactChecks& ListContactChecks::set_contact_id(unsigned long long _contact_id)
{
    contact_id_ = _contact_id;
    return *this;
}

ListContactChecks& ListContactChecks::set_status_handle(const std::string& _status_handle)
{
    status_handle_ = _status_handle;
    return *this;
}

// exec and serialization
std::vector<ListChecksItem> ListContactChecks::exec(OperationContext& _ctx, const std::string& _output_timezone)
{
    _ctx.get_log().debug("ListContactChecks exec() started");
    _ctx.get_log().info(this->to_string());

    try
    {
        const std::string check_alias = "check_";
        const std::string enum_testsuite_alias = "enum_c_t";
        const std::string contact_history_alias = "c_h";

        std::map<std::string, ListChecksItem> checks;

        // selecting basic checks data
        {
            std::vector<std::string> joins;
            std::vector<std::string> wheres;
            Database::QueryParams params;

            if (testsuite_handle_.isset())
            {
                // enum_contact_testsuite is already used by the fixed part of query
                //joins.push_back();
                wheres.push_back(
                        " AND " + enum_testsuite_alias + ".handle=$" + boost::lexical_cast<std::string>(params.size() + 1) +
                        "::varchar ");
                params.push_back(testsuite_handle_.get_value());
            }

            if (contact_id_.isset())
            {
                // contact_historyid is already used by the fixed part of query
                //joins.push_back();
                wheres.push_back(
                    " AND " + contact_history_alias + ".id=$" + boost::lexical_cast<std::string>(params.size() + 1) +
                    "::bigint ");
                params.push_back(contact_id_.get_value());
            }

            if (status_handle_.isset())
            {
                joins.push_back(
                    " JOIN enum_contact_check_status AS enum_c_ch_status ON " +
                    check_alias + ".enum_contact_check_status_id = enum_c_ch_status.id ");
                wheres.push_back(
                    " AND enum_c_ch_status.handle = $" + boost::lexical_cast<std::string>(params.size() + 1) + "::varchar ");
                params.push_back(status_handle_.get_value());
            }

            const std::string timezone_param_order = boost::lexical_cast<std::string>(params.size() + 1);
            params.push_back(_output_timezone);

            const Database::Result dbres = _ctx.get_conn().exec_params(
                    "SELECT " + check_alias + ".handle AS handle_," +
                                check_alias + ".create_time AT TIME ZONE 'utc' "                               // conversion from 'utc'
                                                           "AT TIME ZONE $" + timezone_param_order + "::text " // to _output_timezone
                                                           "AS create_time_," +
                                check_alias + ".contact_history_id AS contact_history_id_," +
                                contact_history_alias + ".id AS contact_id_,"
                                "o_r.name AS contact_handle_," +
                                enum_testsuite_alias + ".handle AS testsuite_handle_," +
                                check_alias + ".update_time AT TIME ZONE 'utc' "                               // conversion from 'utc'
                                                           "AT TIME ZONE $" + timezone_param_order + "::text " // to _output_timezone
                                                           "AS update_time_,"
                                "status.handle AS status_handle_,"
                                "h.valid_from AT TIME ZONE 'utc' "                               // conversion from 'utc'
                                             "AT TIME ZONE $" + timezone_param_order + "::text " // to _output_timezone
                                             "AS last_contact_change_ "
                    "FROM contact_check AS " + check_alias + " "
                    "JOIN enum_contact_testsuite " + enum_testsuite_alias + " "
                         "ON " + enum_testsuite_alias +".id=" + check_alias + ".enum_contact_testsuite_id "
                    "JOIN enum_contact_check_status status "
                         "ON status.id=" + check_alias + ".enum_contact_check_status_id "
                    "JOIN contact_history " + contact_history_alias + " "
                         "ON " + contact_history_alias + ".historyid=" + check_alias + ".contact_history_id "
                    "JOIN object_registry o_r "    // for handle and for historyid which is used below...
                         "ON o_r.id=" + contact_history_alias + ".id "
                    "JOIN history h ON h.id = o_r.historyid " + // ...for last contact changed
                    boost::join(joins, " ") +
                    "WHERE true " + boost::join(wheres, " ") +
                           (max_item_count_.isset() ? " LIMIT " + boost::lexical_cast<std::string>(max_item_count_)
                                                    : ""),
                    params);

            ListChecksItem temp_item;

            for (Database::Result::size_type idx = 0; idx < dbres.size(); ++idx)
            {
                temp_item.check_handle = static_cast<std::string>(dbres[idx]["handle_"]);
                temp_item.contact_history_id = static_cast<unsigned long long>(dbres[idx]["contact_history_id_"]);
                temp_item.contact_handle = static_cast<std::string>(dbres[idx]["contact_handle_"]);
                temp_item.contact_id = static_cast<unsigned long long>(dbres[idx]["contact_id_"]);
                temp_item.local_create_time = boost::posix_time::time_from_string(static_cast<std::string>(dbres[idx]["create_time_"]));
                temp_item.local_update_time = boost::posix_time::time_from_string(static_cast<std::string>(dbres[idx]["update_time_"]));
                temp_item.local_last_contact_update = boost::posix_time::time_from_string(static_cast<std::string>(dbres[idx]["last_contact_change_"]));
                // default - in case it is not set, it remains as NULL
                temp_item.last_test_finished_local_time = Nullable<boost::posix_time::ptime>();
                temp_item.status_handle = static_cast<std::string>(dbres[idx]["status_handle_"]);
                temp_item.testsuite_handle  = static_cast<std::string>(dbres[idx]["testsuite_handle_"]);

                checks.emplace(temp_item.check_handle, temp_item);
            }
        }

        const std::vector<std::string> statuses = LibFred::ContactCheckStatus::get_after_tests_finished();

        // set time when tests finished if it is the current state
        for (auto& it : checks)
        {
            if (std::find(statuses.begin(), statuses.end(), it.second.status_handle) != statuses.end())
            {
                it.second.last_test_finished_local_time = it.second.local_update_time;
            }
        }

        // select time when tests finished if it is already history
        // note: overrides eventually previously set time in previous block
        {
            std::vector<std::string> handles;
            for (const auto& it : checks)
            {
                handles.push_back(it.second.check_handle);
            }

            const Database::Result dbres = _ctx.get_conn().exec_params(
                    // looking for time when check tests finished ~ check.status was set to auto_*
                    "SELECT " + check_alias + ".handle AS handle_,"
                           "MIN(c_c_h.update_time) AT TIME ZONE 'utc' " // conversion from 'utc'
                                                  "AT TIME ZONE $1::text AS tests_finished_ "
                    "FROM contact_check " + check_alias + " "
                    "LEFT JOIN contact_check_history c_c_h "
                         "ON c_c_h.contact_check_id=" + check_alias +".id "
                    "JOIN enum_contact_check_status enum_c_s "
                         "ON enum_c_s.id=c_c_h.enum_contact_check_status_id "
                    "WHERE " + check_alias + ".handle=ANY($2::uuid[]) AND "
                          "(enum_c_s.handle=ANY($3::varchar[]) OR "
                           "c_c_h.contact_check_id IS NULL) "
                    "GROUP BY " + check_alias + ".handle",
                    Database::query_param_list
                        (_output_timezone)
                        ("{" + boost::join(handles, ",") + "}")
                        ("{" + boost::join(statuses, ",") + "}"));
            for (Database::Result::size_type idx = 0; idx < dbres.size(); ++idx)
            {
                checks.at(static_cast<std::string>(dbres[idx]["handle_"])).last_test_finished_local_time =
                        boost::posix_time::time_from_string(static_cast<std::string>(dbres[idx]["tests_finished_"]));
            }
        }

        std::vector<ListChecksItem> result;
        for (const auto& it : checks)
        {
            result.push_back(it.second);
        }

        _ctx.get_log().debug("ListContactChecks executed successfully");

        return result;
    }
    catch (ExceptionStack& ex)
    {
        ex.add_exception_stack_info(this->to_string());
        throw;
    }
}

std::string ListContactChecks::to_string() const
{
    return Util::format_operation_state(
        "ListContactChecks",
        boost::assign::list_of
            (std::make_pair("max_item_count", max_item_count_.print_quoted()))
            (std::make_pair("testsuite_handle", testsuite_handle_.print_quoted()))
            (std::make_pair("contact_id", contact_id_.print_quoted()))
            (std::make_pair("status_handle", status_handle_.print_quoted())));
}

}//namespace LibFred
