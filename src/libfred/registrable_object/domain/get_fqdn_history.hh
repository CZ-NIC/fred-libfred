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

#ifndef GET_FQDN_HISTORY_HH_E9949A39F29740C695A0FECA2C770EE7
#define GET_FQDN_HISTORY_HH_E9949A39F29740C695A0FECA2C770EE7

#include "libfred/registrable_object/get_handle_history.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

using GetFqdnHistory = GetHandleHistory<Object_Type::domain>;

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
