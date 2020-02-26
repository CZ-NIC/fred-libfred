/*
 * Copyright (C) 2020  CZ.NIC, z. s. p. o.
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
#ifndef UTIL_HH_C151AFCAB00F131DB89FBE35E5AAC5A1 // date "+%s" | md5sum | cut -f1 -d" " | tr "[a-f]" "[A-F]" | tr -d "\n"
#define UTIL_HH_C151AFCAB00F131DB89FBE35E5AAC5A1

#include "util/optional_value.hh"

namespace Test {

Optional<unsigned long long> random_log_request_id();

}

#endif // UTIL_HH_C151AFCAB00F131DB89FBE35E5AAC5A1
