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
 *  (get) list (of) contact check
 */

#ifndef LIST_CHECKS_HH_FB6753BD3A1940EB83D5CCB8176D0D4B
#define LIST_CHECKS_HH_FB6753BD3A1940EB83D5CCB8176D0D4B

#include "util/printable.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"

#include "libfred/opcontext.hh"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace LibFred {

/**
 * Return structure of operation @ref ListContactCheck.
 * All times are returned as local @ref ListContactChecks::exec().
 */
struct ListChecksItem
{
    std::string to_string(const std::string& _each_line_prefix = "\t") const;

    std::string check_handle;
    std::string testsuite_handle;
    unsigned long long contact_history_id;
    unsigned long long contact_id;
    std::string contact_handle;
    boost::posix_time::ptime local_create_time;
    boost::posix_time::ptime local_update_time;
    boost::posix_time::ptime local_last_contact_update;
    Nullable<boost::posix_time::ptime> last_test_finished_local_time;
    std::string status_handle;
};

/**
 * Get list of existing record in contact_check table. Has no sideeffects.
 */
class ListContactChecks : public Util::Printable<ListContactChecks>
{
public:
    /**
     * constructor with only mandatory parameter
     * @param _max_item_count     how many records shall be returned at most.
     */
    ListContactChecks() { };

    /**
     * constructor with all available parameters including optional ones
     * @param _max_item_count   how many records shall be returned at most.
     * @param _testsuite_handle filter: only checks with given testsuite are returned
     * @param _contact_id       filter: only checks of given contact (connected by historyid) are returned
     * @param _status_handle    filter: only checks with given status are returned
     */
    ListContactChecks(
            const Optional<unsigned long>& _max_item_count,
            const Optional<std::string>& _testsuite_handle,
            const Optional<unsigned long long>& _contact_id,
            const Optional<std::string>& _status_handle);

    /**
     * setter of optional max_item_count_
     * Call with another value for re-set, no need to unset first.
     */
    ListContactChecks& set_max_item_count(unsigned long _max_item_count);

    /**
     * setter of optional testsuite_handle
     * Call with another value for re-set, no need to unset first.
     */
    ListContactChecks& set_testsuite_handle(const std::string& _testsuite_handle);

    /**
     * setter of optional contact_id
     * Call with another value for re-set, no need to unset first.
     */
    ListContactChecks& set_contact_id(unsigned long long _contact_id);

    /**
     * setter of optional status_handle
     * Call with another value for re-set, no need to unset first.
     */
    ListContactChecks& set_status_handle(const std::string& _status_handle);

    /**
     * commit operation
     * @param _output_timezone Postgres time zone input type (as string e. g. "Europe/Prague") for conversion to local time values.
     * @return Data of existing check in InfoContactCheckOutput structured.
     */
    std::vector<ListChecksItem> exec(const OperationContext& _ctx, const std::string& _output_timezone = "Europe/Prague");

    std::string to_string()const;
private:
    Optional<unsigned long> max_item_count_;
    Optional<std::string> testsuite_handle_;
    Optional<unsigned long long> contact_id_;
    Optional<std::string> status_handle_;
};

}//namespace LibFred

#endif//LIST_CHECKS_HH_FB6753BD3A1940EB83D5CCB8176D0D4B
