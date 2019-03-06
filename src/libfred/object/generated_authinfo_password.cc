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
#include "libfred/object/generated_authinfo_password.hh"

#include <string>
#include <set>

#include <boost/foreach.hpp>

#include "libfred/object/generated_authinfo_password_exception.hh"

namespace LibFred
{
    static inline std::string::size_type min_length() { return 8; }
    static inline std::string::size_type max_length() { return 300; }
    /* This is not the most efficient solution. Expecting to be optimized in case it is an issue (but avoiding less clear and potentially unnecessary premature optimization for now). */
    static inline std::set<char> get_allowed_chars() {
        const std::string allowed_chars_as_string = get_chars_allowed_in_generated_authinfopw();

        return std::set<char>(
            allowed_chars_as_string.begin(),
            allowed_chars_as_string.end()
        );
    }


    GeneratedAuthInfoPassword::GeneratedAuthInfoPassword(const std::string& _password)
    :   password_(_password)
    {
        if (password_.length() < min_length() || password_.length() > max_length()) { throw InvalidGeneratedAuthInfoPassword(); }

        const std::set<char> allowed_chars = get_allowed_chars();

        BOOST_FOREACH(const char& c, password_) {
            if (allowed_chars.find(c) == allowed_chars.end() ) {
                throw InvalidGeneratedAuthInfoPassword();
            }
        }
    }
}
