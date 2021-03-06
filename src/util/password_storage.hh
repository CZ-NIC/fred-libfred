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
#ifndef PASSWORD_STORAGE_HH_17C8BA6B40654F39BCA17B59CD80F53E
#define PASSWORD_STORAGE_HH_17C8BA6B40654F39BCA17B59CD80F53E

#include "util/password_storage/password_data.hh"

#include <string>
#include <stdexcept>

namespace PasswordStorage {

PasswordData encrypt_password_by_preferred_method(
        const std::string& plaintext_password);

bool is_encrypted_by_preferred_method(
        const PasswordData& encrypted_password_data);

struct IncorrectPassword:std::runtime_error
{
    IncorrectPassword():std::runtime_error("incorrect password") { }
};

void check_password(
        const std::string& plaintext_password,
        const PasswordData& encrypted_password_data);

} // namespace PasswordStorage

#endif
