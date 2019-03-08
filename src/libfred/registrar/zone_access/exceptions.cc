/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/zone_access/exceptions.hh"

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

const char* AddRegistrarZoneAccessException::what() const noexcept
{
    return "Failed to add registrar zone access due to an unknown exception.";
}

const char* NonexistentRegistrar::what() const noexcept
{
    return "Registrar doesn't exist.";
}

const char* NonexistentZone::what() const noexcept
{
    return "Zone doesn't exist.";
}

const char* UpdateRegistrarZoneAccessException::what() const noexcept
{
    return "Failed to update registrar zone access due to an unknown exception.";
}

const char* NoUpdateData::what() const noexcept
{
    return "No registrar zone access data for update.";
}

const char* NonexistentZoneAccess::what() const noexcept
{
    return "Registrar zone access doesn't exist.";
}

const char* GetRegistrarZoneAccessException::what() const noexcept
{
    return "Failed to get registrar zone access due to an unknown exception.";
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

