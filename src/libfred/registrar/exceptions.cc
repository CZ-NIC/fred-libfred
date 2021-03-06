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
#include "libfred/registrar/exceptions.hh"

namespace LibFred {
namespace Registrar {

const char* NonExistentRegistrar::what() const noexcept
{
    return "Registrar does not exist.";
}

const char* NoUpdateData::what() const noexcept
{
    return "No registrar data for update.";
}

const char* RegistrarHandleAlreadyExists::what() const noexcept
{
    return "Registrar handle already exists.";
}

const char* VariableSymbolAlreadyExists::what() const noexcept
{
    return "Variable symbol already exists.";
}

const char* UnknownCountryCode::what() const noexcept
{
    return "Registrar has unknown country code.";
}

const char* UpdateRegistrarException::what() const noexcept
{
    return "Failed to update zone due to an unknown exception.";
}

} // namespace LibFred::Registrar
} // namespace LibFred
