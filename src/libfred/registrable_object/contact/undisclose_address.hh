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
#ifndef UNDISCLOSE_ADDRESS_HH_E7C08A53071943B6894428F2E9EE37BF
#define UNDISCLOSE_ADDRESS_HH_E7C08A53071943B6894428F2E9EE37BF

#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace Contact {

void undisclose_address(
        LibFred::OperationContext& _ctx,
        unsigned long long _contact_id,
        const std::string& _registrar_handle);

void undisclose_address_async(
        unsigned long long _contact_id,
        const std::string& _registrar_handle);

} // namespace LibFred::Contact
} // namespace LibFred

#endif//UNDISCLOSE_ADDRESS_HH_E7C08A53071943B6894428F2E9EE37BF
