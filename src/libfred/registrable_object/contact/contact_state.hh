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
#ifndef CONTACT_STATE_HH_F582B2CFFE08EDCAF2A8C04B8F99E131//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CONTACT_STATE_HH_F582B2CFFE08EDCAF2A8C04B8F99E131

#include "libfred/registrable_object/state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

namespace StateFlagName {

extern const char delete_candidate[];
extern const char linked[];

extern const char contact_failed_manual_verification[];
extern const char contact_in_manual_verification[];
extern const char contact_passed_manual_verification[];

extern const char conditionally_identified_contact[];
extern const char identified_contact[];
extern const char validated_contact[];

extern const char mojeid_contact[];

extern const char server_blocked[];
extern const char server_delete_prohibited[];
extern const char server_transfer_prohibited[];
extern const char server_update_prohibited[];

}//namespace LibFred::RegistrableObject::Contact::StateFlagName

using DeleteCandidate = State::Flag::AutomaticExternal<StateFlagName::delete_candidate>;
using Linked = State::Flag::AutomaticExternal<StateFlagName::linked>;

using ContactFailedManualVerification = State::Flag::ManualExternal<StateFlagName::contact_failed_manual_verification>;
using ContactInManualVerification = State::Flag::ManualExternal<StateFlagName::contact_in_manual_verification>;
using ContactPassedManualVerification = State::Flag::ManualExternal<StateFlagName::contact_passed_manual_verification>;

using ConditionallyIdentifiedContact = State::Flag::ManualExternal<StateFlagName::conditionally_identified_contact>;
using IdentifiedContact = State::Flag::ManualExternal<StateFlagName::identified_contact>;
using ValidatedContact = State::Flag::ManualExternal<StateFlagName::validated_contact>;

using MojeidContact = State::Flag::ManualExternal<StateFlagName::mojeid_contact>;

using ServerBlocked = State::Flag::ManualExternal<StateFlagName::server_blocked>;
using ServerDeleteProhibited = State::Flag::ManualExternal<StateFlagName::server_delete_prohibited>;
using ServerTransferProhibited = State::Flag::ManualExternal<StateFlagName::server_transfer_prohibited>;
using ServerUpdateProhibited = State::Flag::ManualExternal<StateFlagName::server_update_prohibited>;

using ContactStateProvide = State::Of<Object_Type::contact,
        DeleteCandidate,
        Linked,
        ContactFailedManualVerification,
        ContactInManualVerification,
        ContactPassedManualVerification,
        ConditionallyIdentifiedContact,
        IdentifiedContact,
        ValidatedContact,
        MojeidContact,
        ServerBlocked,
        ServerDeleteProhibited,
        ServerTransferProhibited,
        ServerUpdateProhibited>;

using ContactState = typename ContactStateProvide::Type;

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//CONTACT_STATE_HH_F582B2CFFE08EDCAF2A8C04B8F99E131
