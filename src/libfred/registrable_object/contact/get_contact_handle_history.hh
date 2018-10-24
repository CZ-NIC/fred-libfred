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

#ifndef GET_CONTACT_HANDLE_HISTORY_HH_440D454C5CDE5BB78279B525CEB01926//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_CONTACT_HANDLE_HISTORY_HH_440D454C5CDE5BB78279B525CEB01926

#include "libfred/registrable_object/get_handle_history.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using GetContactHandleHistory = GetHandleHistory<Object_Type::contact>;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_CONTACT_HANDLE_HISTORY_HH_440D454C5CDE5BB78279B525CEB01926
