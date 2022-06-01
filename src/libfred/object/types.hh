/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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

#ifndef TYPES_HH_C723445AC8D797C0E9FDD9ADD2FFC5EE//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define TYPES_HH_C723445AC8D797C0E9FDD9ADD2FFC5EE

#include <iosfwd>

namespace LibFred {
namespace Object {

class AuthinfoId // TODO: use LibStrong
{
public:
    constexpr explicit AuthinfoId(unsigned long long src)
        : value_{src}
    { }
    AuthinfoId() = default;
    AuthinfoId(const AuthinfoId&) = default;
    AuthinfoId(AuthinfoId&&) = default;
    AuthinfoId& operator=(const AuthinfoId&) = default;
    AuthinfoId& operator=(AuthinfoId&&) = default;
    constexpr unsigned long long operator*() const noexcept { return value_; }
private:
    unsigned long long value_;
    friend constexpr bool operator<(AuthinfoId lhs, AuthinfoId rhs) noexcept
    {
        return lhs.value_ < rhs.value_;
    }
    friend std::ostream& operator<<(std::ostream& out, AuthinfoId value)
    {
        return out << *value;
    }
};

class ObjectId // TODO: use LibStrong
{
public:
    constexpr explicit ObjectId(unsigned long long src)
        : value_{src}
    { }
    ObjectId() = default;
    ObjectId(const ObjectId&) = default;
    ObjectId(ObjectId&&) = default;
    ObjectId& operator=(const ObjectId&) = default;
    ObjectId& operator=(ObjectId&&) = default;
    constexpr unsigned long long operator*() const noexcept { return value_; }
private:
    unsigned long long value_;
    friend constexpr bool operator<(ObjectId lhs, ObjectId rhs) noexcept
    {
        return lhs.value_ < rhs.value_;
    }
    friend std::ostream& operator<<(std::ostream& out, ObjectId value)
    {
        return out << *value;
    }
};

}//namespace LibFred::Object
}//namespace LibFred

#endif//TYPES_HH_C723445AC8D797C0E9FDD9ADD2FFC5EE
