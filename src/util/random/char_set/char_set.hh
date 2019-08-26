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

#ifndef CHAR_SET_HH_5D4EF679B22C4E3FB09F25A256BA59B2
#define CHAR_SET_HH_5D4EF679B22C4E3FB09F25A256BA59B2

namespace Random {
namespace CharSet {

constexpr auto& lower_case_letters()
{
    return "abcdefghijklmnopqrstuvwxyz";
}

constexpr auto& upper_case_letters()
{
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

constexpr auto& letters()
{
    return "abcdefghijklmnopqrstuvwxyz"
           "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

constexpr auto& digits()
{
    return "0123456789";
}

constexpr auto& letters_and_digits()
{
    return "abcdefghijklmnopqrstuvwxyz"
           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
           "0123456789";
}

} // namespace Random::CharSet
} // namespace Random

#endif
