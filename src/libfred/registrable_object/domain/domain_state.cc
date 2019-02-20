/*
 * Copyright (C) 2019  CZ.NIC, z.s.p.o.
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

#include "libfred/registrable_object/domain/domain_state.hh"
#include "util/flagset_impl.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

namespace StateFlagName {

constexpr const char nsset_missing[] = "nssetMissing";
constexpr const char expiration_warning[] = "expirationWarning";
constexpr const char expired[] = "expired";
constexpr const char outzone[] = "outzone";
constexpr const char unguarded[] = "unguarded";
constexpr const char outzone_unguarded_warning[] = "outzoneUnguardedWarning";
constexpr const char outzone_unguarded[] = "outzoneUnguarded";
constexpr const char delete_candidate[] = "deleteCandidate";
constexpr const char delete_warning[] = "deleteWarning";
constexpr const char validation_warning1[] = "validationWarning1";
constexpr const char validation_warning2[] = "validationWarning2";
constexpr const char not_validated[] = "notValidated";

constexpr const char server_outzone_manual[] = "serverOutzoneManual";
constexpr const char server_inzone_manual[] = "serverInzoneManual";
constexpr const char server_registrant_change_prohibited[] = "serverRegistrantChangeProhibited";
constexpr const char server_renew_prohibited[] = "serverRenewProhibited";

constexpr const char server_blocked[] = "serverBlocked";
constexpr const char server_delete_prohibited[] = "serverDeleteProhibited";
constexpr const char server_transfer_prohibited[] = "serverTransferProhibited";
constexpr const char server_update_prohibited[] = "serverUpdateProhibited";

} // namespace LibFred::RegistrableObject::Domain::StateFlagName

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

namespace Util {

using namespace LibFred::RegistrableObject::Domain;

template class FlagSet<DomainStateProvide,
        NssetMissing,
        ExpirationWarning,
        Expired,
        Outzone,
        Unguarded,
        OutzoneUnguardedWarning,
        OutzoneUnguarded,
        DeleteCandidate,
        DeleteWarning,
        ValidationWarning1,
        ValidationWarning2,
        NotValidated,
        ServerOutzoneManual,
        ServerInzoneManual,
        ServerRegistrantChangeProhibited,
        ServerRenewProhibited,
        ServerBlocked,
        ServerDeleteProhibited,
        ServerTransferProhibited,
        ServerUpdateProhibited>;

} // namespace Util
