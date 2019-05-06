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

#include "libfred/registrable_object/domain/get_fqdn_history.hh"

#include "libfred/registrable_object/domain/domain_uuid.hh"
#include "libfred/registrable_object/get_handle_history_impl.hh"

namespace LibFred {
namespace RegistrableObject {

template class GetHandleHistory<Object_Type::domain>;

} // namespace LibFred::RegistrableObject
} // namespace LibFred
