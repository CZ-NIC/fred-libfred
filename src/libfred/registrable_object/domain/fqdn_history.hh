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

#ifndef FQDN_HISTORY_HH_61363BACB4014FDE8A44E9B42F2F81FE
#define FQDN_HISTORY_HH_61363BACB4014FDE8A44E9B42F2F81FE

#include "libfred/registrable_object/handle_history.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

using FqdnHistory = HandleHistory<Object_Type::domain>;

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
