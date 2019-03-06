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
#ifndef CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC

#include "libfred/registrable_object/uuid.hh"
#include "util/db/value.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using ContactUuid = UuidOf<Object_Type::contact>;
using ContactHistoryUuid = HistoryUuidOf<Object_Type::contact>;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

namespace Database {

template <>
LibFred::RegistrableObject::Contact::ContactUuid Value::as()const;

template <>
LibFred::RegistrableObject::Contact::ContactHistoryUuid Value::as()const;

}//namespace Database

#endif//CONTACT_UUID_HH_866B23F359411A87BF70804743F13EDC
