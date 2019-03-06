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
#ifndef GENERATE_AUTHINFO_PASSWORD_HH_8124C724E70D40E0A3D1BC4D95AF19D0
#define GENERATE_AUTHINFO_PASSWORD_HH_8124C724E70D40E0A3D1BC4D95AF19D0

#include "libfred/object/generated_authinfo_password.hh"

namespace LibFred
{
    /**
    * Pseudo-random transfer password generator.
    * @return new generated password of length 8 characters
    * @note uses alphabet "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789" with visually ambiguous characters excluded
    */
    GeneratedAuthInfoPassword generate_authinfo_pw();
}

#endif
