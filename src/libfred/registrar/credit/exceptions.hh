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
#ifndef EXCEPTIONS_HH_9239B0F470C541539D24AF0F2163DA1D
#define EXCEPTIONS_HH_9239B0F470C541539D24AF0F2163DA1D

#include <exception>

namespace LibFred {
namespace Registrar {
namespace Credit {

struct CreateTransactionException : std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentRegistrar : std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentZoneAccess : std::exception
{
    const char* what() const noexcept override;
};

struct ZoneAccessException : std::exception
{
    const char* what() const noexcept override;
};

} // namespace LibFred::Registrar::Credit
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
