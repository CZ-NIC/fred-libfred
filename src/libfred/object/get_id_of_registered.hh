/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
 *  header of get_objects_id method
 */

#ifndef GET_ID_OF_REGISTERED_HH_5FED292227824A75A2E624DDB696B907
#define GET_ID_OF_REGISTERED_HH_5FED292227824A75A2E624DDB696B907

#include "libfred/object/object_type.hh"
#include "libfred/opcontext.hh"

#include <string>
#include <exception>

namespace LibFred {

struct UnknownObject : std::exception
{
    const char* what() const noexcept;
};

template <Object_Type::Enum object_type>
unsigned long long get_id_of_registered(
        const OperationContext& ctx,
        const std::string& handle);

} // namespace LibFred

#endif
