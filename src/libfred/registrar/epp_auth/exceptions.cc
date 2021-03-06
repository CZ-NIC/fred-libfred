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
#include "libfred/registrar/epp_auth/exceptions.hh"

namespace LibFred {
namespace Registrar {
namespace EppAuth {

const char* AddRegistrarEppAuthException::what() const noexcept
{
    return "Failed to add registrar EPP authentication due to an unknown exception.";
}

const char* NonexistentRegistrar::what() const noexcept
{
    return "Registrar does not exist.";
}

const char* DeleteRegistrarEppAuthException::what() const noexcept
{
    return "Failed to delete registrar EPP authentication due to an unknown exception.";
}

const char* NonexistentRegistrarEppAuth::what() const noexcept
{
    return "Registrar EPP authentication does not exist.";
}

const char* UpdateRegistrarEppAuthException::what() const noexcept
{
    return "Failed to update registrar EPP authentication due to an unknown exception.";
}

const char* NoUpdateData::what() const noexcept
{
    return "No EPP authentication data of registrar for update.";
}

const char* GetRegistrarEppAuthException::what() const noexcept
{
    return "Failed to get registrar EPP authentication due to an unknown exception.";
}

const char* CloneRegistrarEppAuthException::what() const noexcept
{
    return "Failed to clone registrar EPP authentication due to an unknown exception.";
}

const char* DuplicateCertificate::what() const noexcept
{
    return "Registrar certificate already exists.";
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred
