/*
 * Copyright (C) 2014  CZ.NIC, z.s.p.o.
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
 *  registrar info output structure
 */

#ifndef INFO_REGISTRAR_OUTPUT_HH_B02F3193B41A4A21ACFC4224FE193452
#define INFO_REGISTRAR_OUTPUT_HH_B02F3193B41A4A21ACFC4224FE193452

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"
#include "libfred/registrar/info_registrar_data.hh"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
* Element of registrar info data.
*/
struct InfoRegistrarOutput : Util::Printable<InfoRegistrarOutput>
{
    /**
    * Empty constructor of the registrar info data structure.
    */
    InfoRegistrarOutput() {}

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;

    /**
    * Equality of the registrar info data structure operator. Compares only InfoRegistrarData member.
    * @param rhs is right hand side of the registrar data comparison
    * @return true if equal, false if not
    */
    bool operator==(const InfoRegistrarOutput& rhs) const;

    /**
    * Inequality of the registrar info data structure operator. Compares only InfoRegistrarData member.
    * @param rhs is right hand side of the registrar data comparison
    * @return true if not equal, false if equal
    */
    bool operator!=(const InfoRegistrarOutput& rhs) const;

    InfoRegistrarData info_registrar_data;/**< data of the registrar */
    boost::posix_time::ptime utc_timestamp;/**< timestamp of getting the registrar data in UTC */
};

}//namespace LibFred

#endif//INFO_REGISTRAR_OUTPUT_HH_B02F3193B41A4A21ACFC4224FE193452
