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
#include "libfred/object/generate_authinfo_password.hh"
#include "util/random/random.hh"

#include <stdexcept>
#include <string>
#include <sys/time.h>
#include <utility>

namespace LibFred
{
    GeneratedAuthInfoPassword generate_authinfo_pw() {
        const int authinfo_pw_length = 8;
        const std::string result = Random::Generator().get_seq(
            get_chars_allowed_in_generated_authinfopw(),
            authinfo_pw_length);
        return GeneratedAuthInfoPassword(std::move(result));
    }
}
