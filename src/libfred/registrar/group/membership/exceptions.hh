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
/**
 *  @file
 *  exceptions
 */

#ifndef EXCEPTIONS_HH_DC47802337AF4A2493F20AC3E084CA84
#define EXCEPTIONS_HH_DC47802337AF4A2493F20AC3E084CA84

#include <exception>

struct WrongIntervalOrder : std::exception
{
    const char* what() const noexcept override
    {
        return "date from is later than date to";
    }
};

struct IntervalIntersection : std::exception
{
    const char* what() const noexcept override
    {
        return "new membership starts when old one is active";
    }
};

struct MembershipStartChange : std::exception
{
    const char* what() const noexcept override
    {
        return "membership starting date must not be changed";
    }
};

struct WrongMembershipEnd : std::exception
{
    const char* what() const noexcept override
    {
        return "membership infiniteness must not be altered";
    }
};

struct WrongRegistrar : std::exception
{
    const char* what() const noexcept override
    {
        return "this membership has different registrar";
    }
};

struct WrongGroup : std::exception
{
    const char* what() const noexcept override
    {
        return "this membership has different registrar group";
    }
};

struct MembershipNotFound : std::exception
{
    const char* what() const noexcept override
    {
        return "no active membership with given registrar and group found";
    }
};

#endif
