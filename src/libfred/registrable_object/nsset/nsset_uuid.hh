/*
 * Copyright (C) 2019  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NSSET_UUID_HH_46EE0C618ADE42CA8044B37B677D950F
#define NSSET_UUID_HH_46EE0C618ADE42CA8044B37B677D950F

#include "libfred/registrable_object/uuid.hh"
#include "util/db/value.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Nsset {

using NssetUuid = UuidOf<Object_Type::nsset>;
using NssetHistoryUuid = HistoryUuidOf<Object_Type::nsset>;

} // namespace LibFred::RegistrableObject::Nsset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

namespace Database {

template <>
LibFred::RegistrableObject::Nsset::NssetUuid Value::as() const;

template <>
LibFred::RegistrableObject::Nsset::NssetHistoryUuid Value::as() const;

} // namespace Database

#endif
