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

#include "libfred/registrable_object/contact/get_contact_state.hh"
#include "libfred/registrable_object/get_state_impl.hh"

namespace LibFred {
namespace RegistrableObject {

using namespace Contact;

template <>
bool does_correspond<DeleteCandidate>(const std::string& flag_name) { return flag_name == "deleteCandidate"; }
template <>
bool does_correspond<Linked>(const std::string& flag_name) { return flag_name == "linked"; }
template <>
bool does_correspond<ContactFailedManualVerification>(const std::string& flag_name) { return flag_name == "contactFailedManualVerification"; }
template <>
bool does_correspond<ContactInManualVerification>(const std::string& flag_name) { return flag_name == "contactInManualVerification"; }
template <>
bool does_correspond<ContactPassedManualVerification>(const std::string& flag_name) { return flag_name == "contactPassedManualVerification"; }
template <>
bool does_correspond<ConditionallyIdentifiedContact>(const std::string& flag_name) { return flag_name == "conditionallyIdentifiedContact"; }
template <>
bool does_correspond<IdentifiedContact>(const std::string& flag_name) { return flag_name == "identifiedContact"; }
template <>
bool does_correspond<ValidatedContact>(const std::string& flag_name) { return flag_name == "validatedContact"; }
template <>
bool does_correspond<MojeidContact>(const std::string& flag_name) { return flag_name == "mojeidContact"; }
template <>
bool does_correspond<ServerBlocked>(const std::string& flag_name) { return flag_name == "serverBlocked"; }
template <>
bool does_correspond<ServerDeleteProhibited>(const std::string& flag_name) { return flag_name == "serverDeleteProhibited"; }
template <>
bool does_correspond<ServerTransferProhibited>(const std::string& flag_name) { return flag_name == "serverTransferProhibited"; }
template <>
bool does_correspond<ServerUpdateProhibited>(const std::string& flag_name) { return flag_name == "serverUpdateProhibited"; }

template class GetStateById<ContactState>;
template class GetState<GetStateById<ContactState>, ContactState>;

template class GetStateByHandle<ContactState>;
template class GetState<GetStateByHandle<ContactState>, ContactState>;

}//namespace LibFred::RegistrableObject
}//namespace LibFred
