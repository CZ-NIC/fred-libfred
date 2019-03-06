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
 *  nsset info output structure
 */

#ifndef INFO_NSSET_OUTPUT_HH_3B3A6EFB6BB4457A999D7E84BD9FA805
#define INFO_NSSET_OUTPUT_HH_3B3A6EFB6BB4457A999D7E84BD9FA805

#include "libfred/registrable_object/nsset/info_nsset_data.hh"

#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
* Element of nsset info data.
*/
struct InfoNssetOutput : Util::Printable<InfoNssetOutput>
{
    /**
    * Empty constructor of the nsset info data structure.
    */
    InfoNssetOutput()
    {}

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;

    /**
    * Equality of the nsset info data structure operator. Compares only InfoNssetData member.
    * @param rhs is right hand side of the nsset data comparison
    * @return true if equal, false if not
    */
    bool operator==(const InfoNssetOutput& rhs) const;

    /**
    * Inequality of the nsset info data structure operator. Compares only InfoNssetData member.
    * @param rhs is right hand side of the nsset data comparison
    * @return true if not equal, false if equal
    */
    bool operator!=(const InfoNssetOutput& rhs) const;

    InfoNssetData info_nsset_data;/**< data of the nsset */

    boost::posix_time::ptime utc_timestamp;/**< timestamp of getting the nsset data in UTC */

    Nullable<unsigned long long> next_historyid; /**< next historyid of the nsset history*/
    boost::posix_time::ptime history_valid_from;/**< history data valid from time */
    Nullable<boost::posix_time::ptime> history_valid_to;/**< history data valid to time, null means open end */
    Nullable<unsigned long long> logd_request_id; /**< id of the request that changed nsset data*/
};

}//namespace LibFred

#endif//INFO_NSSET_OUTPUT_HH_3B3A6EFB6BB4457A999D7E84BD9FA805
