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

#ifndef EXCEPTIONS_HH_A96C589FC5F14474872247B053482D69
#define EXCEPTIONS_HH_A96C589FC5F14474872247B053482D69

#include <exception>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

struct AddRegistrarZoneAccessException : virtual std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentRegistrar : virtual std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentZone : virtual std::exception
{
    const char* what() const noexcept override;
};

struct UpdateRegistrarZoneAccessException : virtual std::exception
{
    const char* what() const noexcept override;
};

struct NoUpdateData : virtual std::exception
{
    const char* what() const noexcept override;
};

struct NonexistentZoneAccess : virtual std::exception
{
    const char* what() const noexcept override;
};

struct OverlappingZoneAccessRange : virtual std::exception
{
    const char* what() const noexcept override;
};

struct InvalidDateFrom : virtual std::exception
{
    const char* what() const noexcept override;
};

struct InvalidZoneAccessPeriod : virtual std::exception { };

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
