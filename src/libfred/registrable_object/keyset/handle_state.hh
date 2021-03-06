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
/**
 *  @file
 */

#ifndef HANDLE_STATE_HH_8DF0E6E964534C0BAB8C3ED6EC9DA162
#define HANDLE_STATE_HH_8DF0E6E964534C0BAB8C3ED6EC9DA162

namespace LibFred {
namespace Keyset {

struct HandleState
{
    enum Registrability
    {
        registered,
        in_protection_period,
        available,
    };

    enum SyntaxValidity
    {
        valid,
        invalid,
    };
};

} // namespace LibFred::Keyset
} // namespace LibFred

#endif
