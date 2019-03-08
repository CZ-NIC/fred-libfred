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
#ifndef STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <utility>

namespace Util {

template <typename Type, typename Name> class StrongType;

template <typename N, typename T>
constexpr N make_strong(const T&);

template <typename N, typename T>
constexpr N make_strong(T&&);

template <typename T, typename N>
constexpr const T& get_raw_value_from(const StrongType<T, N>&);

template <typename T, typename N>
T&& get_raw_value_from(StrongType<T, N>&&);

template <typename T, typename N>
class StrongType
{
public:
    using UnderlyingType = T;
    StrongType() = default;
    ~StrongType() = default;
    StrongType(const StrongType&) = default;
    StrongType(StrongType&&) = default;
    StrongType& operator=(const StrongType&) = default;
    StrongType& operator=(StrongType&&) = default;
private:
    explicit constexpr StrongType(const T& src) : value_(src) { }
    explicit StrongType(T&& src) : value_(std::move(src)) { }
    StrongType& operator=(const T& src) { value_ = src; return *this; }
    StrongType& operator=(T&& src) { std::swap(value_, src); return *this; }
    T value_;
    friend StrongType make_strong<>(const T&);
    friend StrongType make_strong<>(T&&);
    friend const T& get_raw_value_from<>(const StrongType&);
    friend T&& get_raw_value_from<>(StrongType&&);
    friend std::ostream& operator<<(std::ostream& out, const StrongType& src)
    {
        return out << src.value_;
    }
};

template <typename N, typename T>
constexpr N make_strong(const T& src)
{
    static_assert(std::is_same<decltype(get_raw_value_from(N())), T&&>::value,
                  "N must be strong type based on T");
    return N(src);
}

template <typename N, typename T>
constexpr N make_strong(T&& src)
{
    static_assert(std::is_same<decltype(get_raw_value_from(N())), T&&>::value,
                  "N must be strong type based on T");
    return N(std::move(src));
}

template <typename T, typename N>
std::string strong_to_string(const StrongType<T, N>& src)
{
    std::ostringstream out;
    out << src;
    return out.str();
}

template <typename T, typename N>
constexpr const T& get_raw_value_from(const StrongType<T, N>& src)
{
    return src.value_;
}

template <typename T, typename N>
T&& get_raw_value_from(StrongType<T, N>&& src)
{
    return std::move(src.value_);
}

template <typename S, typename T, typename N>
S strong_type_cast(const StrongType<T, N>& src)
{
    static_assert(std::is_same<decltype(get_raw_value_from(S())), T&&>::value,
                  "S must be strong type based on T");
    return Util::make_strong<S>(get_raw_value_from(src));
}

template <typename S, typename T, typename N>
S strong_type_cast(StrongType<T, N>&& src)
{
    static_assert(std::is_same<decltype(get_raw_value_from(S())), T&&>::value,
                  "S must be strong type based on T");
    return Util::make_strong<S>(get_raw_value_from(std::move(src)));
}

}//namespace Util

#endif//STRONG_TYPE_HH_3A4059C222372A9EB2F4A653236A5730
