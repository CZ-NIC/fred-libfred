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
 *  keyset info
 */

#ifndef INFO_KEYSET_OUTPUT_HH_2906766BA0734F9394B24C5FBD8F158C
#define INFO_KEYSET_OUTPUT_HH_2906766BA0734F9394B24C5FBD8F158C

#include "libfred/registrable_object/keyset/info_keyset_data.hh"

#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
* Element of keyset info data.
*/
struct InfoKeysetOutput : Util::Printable<InfoKeysetOutput>
{
    /**
    * Empty constructor of the keyset info data structure.
    */
    InfoKeysetOutput()
    {}

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;

    /**
    * Equality of the keyset info data structure operator. Compares only InfoKeysetData member.
    * @param rhs is right hand side of the keyset data comparison
    * @return true if equal, false if not
    */
    bool operator==(const InfoKeysetOutput& rhs) const;

    /**
    * Inequality of the keyset info data structure operator. Compares only InfoKeysetData member.
    * @param rhs is right hand side of the keyset data comparison
    * @return true if not equal, false if equal
    */
    bool operator!=(const InfoKeysetOutput& rhs) const;

    InfoKeysetData info_keyset_data;/**< data of the keyset */

    boost::posix_time::ptime utc_timestamp;/**< timestamp of getting the keyset data in UTC */

    Nullable<unsigned long long> next_historyid; /**< next historyid of the keyset history*/
    boost::posix_time::ptime history_valid_from;/**< history data valid from time in local time zone viz @ref local_timestamp_pg_time_zone_name */
    Nullable<boost::posix_time::ptime> history_valid_to;/**< history data valid to time in local time zone viz @ref local_timestamp_pg_time_zone_name, null means open end */
    Nullable<unsigned long long> logd_request_id; /**< id of the request that changed keyset data*/
};

}//namespace LibFred

#endif//INFO_KEYSET_OUTPUT_HH_2906766BA0734F9394B24C5FBD8F158C
