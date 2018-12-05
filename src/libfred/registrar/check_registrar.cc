/*
 * Copyright (C) 2015  CZ.NIC, z.s.p.o.
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
 *  registrar check
 */

#include "libfred/registrar/check_registrar.hh"
#include "util/util.hh"

#include <boost/regex.hpp>

namespace LibFred {

CheckRegistrar::CheckRegistrar(const std::string& handle)
    : handle_(handle)
{}

bool CheckRegistrar::is_invalid_handle()const
{
    static const boost::regex registrar_handle_syntax("[a-zA-Z0-9](-?[a-zA-Z0-9])*");
    const bool length_in_range = (3 <= handle_.length()) && (handle_.length() <= 16);
    const bool handle_is_valid = length_in_range && boost::regex_match(handle_, registrar_handle_syntax);
    return !handle_is_valid;
}

std::string CheckRegistrar::to_string()const
{
    return Util::format_operation_state(
            "CheckRegistrar",
            Util::vector_of<std::pair<std::string, std::string>>(std::make_pair("handle", handle_)));
}

}//namespace LibFred
