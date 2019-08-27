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

#ifndef UTILS_HH_70DE1039155F480CBFAA6E2F13D53458
#define UTILS_HH_70DE1039155F480CBFAA6E2F13D53458

#include <type_traits>

namespace Random {
namespace Details {

template<typename T>
using IsCharStrict = std::integral_constant<bool,
                         std::is_same<T, char>::value ||
                         std::is_same<T, char16_t>::value ||
                         std::is_same<T, char32_t>::value ||
                         std::is_same<T, wchar_t>::value>;

template<typename T>
using IsChar = IsCharStrict<std::remove_cv_t<T>>;

} // namespace Random::Details
} // namespace Random

#endif
