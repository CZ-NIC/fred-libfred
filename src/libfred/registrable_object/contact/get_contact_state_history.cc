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

#include "libfred/registrable_object/contact/get_contact_state_history.hh"
#include "libfred/registrable_object/get_state_history_impl.hh"
#include "libfred/registrable_object/operation_by_id_impl.hh"
#include "libfred/registrable_object/operation_by_handle_impl.hh"
#include "libfred/registrable_object/operation_by_uuid_impl.hh"

namespace LibFred {
namespace RegistrableObject {

using namespace Contact;

template class OperationById<GetStateHistory, ContactState>;
template class GetStateHistory<OperationById<GetStateHistory, ContactState>, ContactState>;

template class OperationByHandle<GetStateHistory, ContactState>;
template class GetStateHistory<OperationByHandle<GetStateHistory, ContactState>, ContactState>;

template class OperationByUUID<GetStateHistory, ContactState>;
template class GetStateHistory<OperationByUUID<GetStateHistory, ContactState>, ContactState>;

}//namespace LibFred::RegistrableObject
}//namespace LibFred
