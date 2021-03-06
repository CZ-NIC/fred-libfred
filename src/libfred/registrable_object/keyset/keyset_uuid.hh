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
#ifndef KEYSET_UUID_HH_4B362FEFED8F463FA5DD2A5FBF9E0B9C
#define KEYSET_UUID_HH_4B362FEFED8F463FA5DD2A5FBF9E0B9C

#include "libfred/registrable_object/uuid.hh"
#include "util/db/value.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Keyset {

using KeysetUuid = UuidOf<Object_Type::keyset>;
using KeysetHistoryUuid = HistoryUuidOf<Object_Type::keyset>;

} // namespace LibFred::RegistrableObject::Keyset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

namespace Database {

template <>
LibFred::RegistrableObject::Keyset::KeysetUuid Value::as() const;

template <>
LibFred::RegistrableObject::Keyset::KeysetHistoryUuid Value::as() const;

} // namespace Database

#endif
