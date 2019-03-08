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
#include "libfred/zone/util.hh"

#include <regex>

namespace LibFred {
namespace Zone {

bool is_enum_zone(const std::string& _fqdn)
{
    const std::regex enum_regex("^([^.]+\\.)*e164\\.arpa\\.?$", std::regex::icase);
    return std::regex_match(_fqdn, enum_regex);
}

} // namespace LibFred::Zone
} // namespace LibFred
