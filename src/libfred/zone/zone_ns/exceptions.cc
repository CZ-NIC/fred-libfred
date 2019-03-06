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
#include "libfred/zone/zone_ns/exceptions.hh"

namespace LibFred {
namespace Zone {

const char* CreateZoneNsException::what() const noexcept
{
    return "Failed to create zone ns due to an unknown exception.";
}

const char* InfoZoneNsException::what() const noexcept
{
    return "Failed to get zone ns info due to an unknown exception.";
}

const char* NonExistentZoneNs::what() const noexcept
{
    return "Zone ns does not exist.";
}

const char* NoZoneNsData::what() const noexcept
{
    return "No zone ns data for update.";
}

const char* UpdateZoneNsException::what() const noexcept
{
    return "Failed to update zone ns due to an unknown exception.";
}

} // namespace LibFred::Zone
} // namespace LibFred
