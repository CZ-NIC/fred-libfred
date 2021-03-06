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
#ifndef EXCEPTIONS_HH_972EE3B938CE4074852072D1C880B936
#define EXCEPTIONS_HH_972EE3B938CE4074852072D1C880B936

#include <exception>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

struct AddRegistrarEppAuthException : std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentRegistrar : std::exception
{
    const char* what() const noexcept override;
};

struct DeleteRegistrarEppAuthException : std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentRegistrarEppAuth : std::exception
{
    const char* what() const noexcept override;
};

struct UpdateRegistrarEppAuthException : std::exception
{
    const char* what() const noexcept override;
};

struct NoUpdateData : std::exception
{
    const char* what() const noexcept override;
};

struct GetRegistrarEppAuthException : std::exception
{
    const char* what() const noexcept override;
};

struct CloneRegistrarEppAuthException : std::exception
{
    const char* what() const noexcept override;
};

struct DuplicateCertificate : std::exception
{
    const char* what() const noexcept override;
};

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
