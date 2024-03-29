/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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

#include "libfred/registrable_object/contact/contact_state.hh"
#include "util/flagset_impl.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {
namespace StateFlagName {

constexpr const char delete_candidate[] = "deleteCandidate";
constexpr const char linked[] = "linked";

constexpr const char contact_failed_manual_verification[] = "contactFailedManualVerification";
constexpr const char contact_in_manual_verification[] = "contactInManualVerification";
constexpr const char contact_passed_manual_verification[] = "contactPassedManualVerification";

constexpr const char conditionally_identified_contact[] = "conditionallyIdentifiedContact";
constexpr const char identified_contact[] = "identifiedContact";
constexpr const char validated_contact[] = "validatedContact";

constexpr const char mojeid_contact[] = "mojeidContact";

constexpr const char server_blocked[] = "serverBlocked";
constexpr const char server_delete_prohibited[] = "serverDeleteProhibited";
constexpr const char server_transfer_prohibited[] = "serverTransferProhibited";
constexpr const char server_update_prohibited[] = "serverUpdateProhibited";

constexpr const char server_contact_name_change_prohibited[] = "serverContactNameChangeProhibited";
constexpr const char server_contact_organization_change_prohibited[] = "serverContactOrganizationChangeProhibited";
constexpr const char server_contact_ident_change_prohibited[] = "serverContactIdentChangeProhibited";
constexpr const char server_contact_permanent_address_change_prohibited[] = "serverContactPermanentAddressChangeProhibited";

}//namespace LibFred::RegistrableObject::Contact::StateFlagName
}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

namespace Util {

using namespace LibFred::RegistrableObject::Contact;

template class FlagSet<ContactStateProvide,
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
        ServerUpdateProhibited,
        ServerContactNameChangeProhibited,
        ServerContactOrganizationChangeProhibited,
        ServerContactIdentChangeProhibited,
        ServerContactPermanentAddressChangeProhibited>;

}//namespace Util
