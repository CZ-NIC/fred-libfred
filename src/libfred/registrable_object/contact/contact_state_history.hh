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
#ifndef CONTACT_STATE_HISTORY_HH_4A3B56DF43A8017423C215283116A15C//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CONTACT_STATE_HISTORY_HH_4A3B56DF43A8017423C215283116A15C

#include "libfred/registrable_object/state_history.hh"
#include "libfred/registrable_object/contact/contact_state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using ContactStateHistory = StateHistory<ContactState>;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//CONTACT_STATE_HISTORY_HH_4A3B56DF43A8017423C215283116A15C
