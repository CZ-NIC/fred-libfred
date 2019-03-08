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
#ifndef DOMAIN_UUID_HH_B94B1366DDB14651B4F0AF955FB078D2
#define DOMAIN_UUID_HH_B94B1366DDB14651B4F0AF955FB078D2

#include "libfred/registrable_object/uuid.hh"
#include "util/db/value.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

using DomainUuid = UuidOf<Object_Type::domain>;
using DomainHistoryUuid = HistoryUuidOf<Object_Type::domain>;

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

namespace Database {

template <>
LibFred::RegistrableObject::Domain::DomainUuid Value::as() const;

template <>
LibFred::RegistrableObject::Domain::DomainHistoryUuid Value::as() const;

} // namespace Database

#endif
