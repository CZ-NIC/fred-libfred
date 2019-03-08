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
#ifndef RANDOM_HH_287356A4FEE34BE9BE4B8C9B92BA9B63
#define RANDOM_HH_287356A4FEE34BE9BE4B8C9B92BA9B63

#include <string>

namespace Random {

int integer(int _min, int _max);

std::string string_from(unsigned _len, const std::string& _allowed);

std::string string_lower_alpha(unsigned _len);

std::string string_upper_alpha(unsigned _len);

std::string string_alpha(unsigned _len);

std::string string_alphanum(unsigned _len);

} // namespace Random

#endif /*RANDOM_UTIL_H_*/
