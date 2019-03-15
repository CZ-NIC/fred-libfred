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

#ifndef CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398

#include <string>
#include <type_traits>
#include <utility>

namespace Util {

template <typename T>
auto case_insensitive_equal_to(T&&);

template <typename T>
class CaseInsensitiveEqualTo
{
public:
    bool operator()(const std::string& lhs, const std::string& rhs)const;
private:
    explicit CaseInsensitiveEqualTo(std::add_lvalue_reference_t<T> db_conn)
        : db_conn_(db_conn) { }
    std::add_lvalue_reference_t<T> db_conn_;
    template <typename D>
    friend auto case_insensitive_equal_to(D&&);
};

template <typename T>
auto case_insensitive_equal_to(T&& db_conn)
{
    return CaseInsensitiveEqualTo<std::remove_reference_t<T>>(std::forward<T>(db_conn));
}

template <>
class CaseInsensitiveEqualTo<void>
{
public:
    bool operator()(const std::string& lhs, const std::string& rhs)const;
};

CaseInsensitiveEqualTo<void> case_insensitive_equal_to();

}//namespace Util

#endif//CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398
