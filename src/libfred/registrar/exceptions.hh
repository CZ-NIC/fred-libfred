/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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

#ifndef EXCEPTIONS_HH_2595C4AB83554D95BAD1348002A17E35
#define EXCEPTIONS_HH_2595C4AB83554D95BAD1348002A17E35

#include <exception>

namespace LibFred {
namespace Registrar {

struct NonExistentRegistrar : std::exception
{
    const char* what() const noexcept override;
};

struct NoUpdateData : std::exception
{
    const char* what() const noexcept override;
};

struct RegistrarHandleAlreadyExists : std::exception
{
    const char* what() const noexcept override;
};

struct VariableSymbolAlreadyExists : std::exception
{
    const char* what() const noexcept override;
};

struct UnknownCountryCode : std::exception
{
    const char* what() const noexcept override;
};

struct UpdateRegistrarException : std::exception
{
    const char* what() const noexcept override;
};

struct InvalidAttribute : UpdateRegistrarException
{
    virtual const char* attribute_name() const noexcept = 0;
};

} // namespace LibFred::Registrar
} // namespace LibFred

#endif//EXCEPTIONS_HH_2595C4AB83554D95BAD1348002A17E35
