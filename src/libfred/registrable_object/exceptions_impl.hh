/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
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

#ifndef EXCEPTIONS_IMPL_HH_9049BC6E62E7DA9FD41C65F44126F98C//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define EXCEPTIONS_IMPL_HH_9049BC6E62E7DA9FD41C65F44126F98C

#include "libfred/registrable_object/exceptions.hh"

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
ObjectNotFound<o>::ObjectNotFound()
    : std::runtime_error(Conversion::Enums::to_db_handle(object_type) + " not found")
{ }

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//EXCEPTIONS_IMPL_HH_9049BC6E62E7DA9FD41C65F44126F98C
