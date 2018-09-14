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

#ifndef CONTACT_STATUS_HH_F582B2CFFE08EDCAF2A8C04B8F99E131//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CONTACT_STATUS_HH_F582B2CFFE08EDCAF2A8C04B8F99E131

#include "libfred/registrable_object/status.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

using DeleteCandidate = StatusFlag::AutomaticExternal<struct DeleteCandidateFlag>;
using Linked = StatusFlag::AutomaticExternal<struct LinkedFlag>;

using ContactFailedManualVerification = StatusFlag::ManualExternal<struct ContactFailedManualVerificationFlag>;
using ContactInManualVerification = StatusFlag::ManualExternal<struct ContactInManualVerificationFlag>;
using ContactPassedManualVerification = StatusFlag::ManualExternal<struct ContactPassedManualVerificationFlag>;

using ConditionallyIdentifiedContact = StatusFlag::ManualExternal<struct ConditionallyIdentifiedContactFlag>;
using IdentifiedContact = StatusFlag::ManualExternal<struct IdentifiedContactFlag>;
using ValidatedContact = StatusFlag::ManualExternal<struct ValidatedContactFlag>;

using MojeidContact = StatusFlag::ManualExternal<struct MojeidContactFlag>;

using ServerBlocked = StatusFlag::ManualExternal<struct ServerBlockedFlag>;
using ServerDeleteProhibited = StatusFlag::ManualExternal<struct ServerDeleteProhibitedFlag>;
using ServerTransferProhibited = StatusFlag::ManualExternal<struct ServerTransferProhibitedFlag>;
using ServerUpdateProhibited = StatusFlag::ManualExternal<struct ServerUpdateProhibitedFlag>;

using ContactStatus = Status<Object_Type::contact,
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

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//CONTACT_STATUS_HH_F582B2CFFE08EDCAF2A8C04B8F99E131
