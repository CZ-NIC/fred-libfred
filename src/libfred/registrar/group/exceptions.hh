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

#ifndef EXCEPTIONS_HH_4C76FE82FBE24642BB790BAA45297A4C
#define EXCEPTIONS_HH_4C76FE82FBE24642BB790BAA45297A4C

#include <exception>

struct GroupExists : std::exception
{
    const char* what() const noexcept override
    {
        return "registrar group with this name still exists";
    }
};

struct EmptyGroupName : std::exception
{
    const char* what() const noexcept override
    {
        return "registrar group name has no text";
    }
};

struct NonemptyGroupDelete : std::exception
{
    const char* what() const noexcept override
    {
        return "the registrar group to delete still has some members";
    }
};

struct AlreadyCancelled : std::exception
{
    const char* what() const noexcept override
    {
        return "registrar group is already cancelled";
    }
};

#endif
