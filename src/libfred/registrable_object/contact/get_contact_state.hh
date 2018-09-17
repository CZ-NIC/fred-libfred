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

#ifndef GET_CONTACT_STATE_HH_F0AFD789AEE855993F8E1F55F62D27C0//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_CONTACT_STATE_HH_F0AFD789AEE855993F8E1F55F62D27C0

#include "libfred/registrable_object/get_state.hh"
#include "libfred/registrable_object/contact/contact_state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using GetContactStateById = GetStateById<ContactState>;
using GetContactStateByHandle = GetStateByHandle<ContactState>;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_CONTACT_STATE_HH_F0AFD789AEE855993F8E1F55F62D27C0
