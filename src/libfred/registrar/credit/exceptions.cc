/*
 * Copyright (C) 2019  CZ.NIC, z.s.p.o.
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

#include "src/libfred/registrar/credit/exceptions.hh"

namespace LibFred {
namespace Registrar {
namespace Credit {

const char* CreateTransactionException::what() const noexcept
{
    return "Failed to create registrar credit transaction due to an unknown exception.";
}

const char* NonexistentRegistrar::what() const noexcept
{
    return "Registrar does not exist.";
}

const char* NonexistentZone::what() const noexcept
{
    return "Zone does not exist.";
}

const char* NonexistentZoneAccess::what() const noexcept
{
    return "Registrar hasn't a zone access.";
}

const char* ZoneAccessException::what() const noexcept
{
    return "Failed to create transaction due to getting a zone access.";
}

} // namespace LibFred::Registrar::Credit
} // namespace LibFred::Registrar
} // namespace LibFred
