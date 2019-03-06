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
#ifndef GENERATED_AUTHINFO_PASSWORD_HH_6D0E2560F0FD407DBCBA5E6F072AB663
#define GENERATED_AUTHINFO_PASSWORD_HH_6D0E2560F0FD407DBCBA5E6F072AB663

#include <string>

namespace LibFred
{
    /**
     * @returns character allowed to be used in GeneratedAuthInfoPassword
     *
     * Characters should be visually distinct to prevent confusion (e. g. 'l', 'I', '1' are not allowed).
     */
    inline std::string get_chars_allowed_in_generated_authinfopw() {
        return "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789";
    }

    /**
     * Represents newly generated authinfo that is valid according to our rules.
     */
    struct GeneratedAuthInfoPassword {
        /** @throws InvalidGeneratedAuthInfoPassword */
        explicit GeneratedAuthInfoPassword(const std::string& _password);
        const std::string password_;
    };
}

#endif
