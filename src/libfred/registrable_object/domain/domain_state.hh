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

#ifndef DOMAIN_STATE_HH_D39D05ECA14A44E4A4697E0E5F8BB54B
#define DOMAIN_STATE_HH_D39D05ECA14A44E4A4697E0E5F8BB54B

#include "libfred/registrable_object/state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

namespace StateFlagName {

extern const char nsset_missing[];
extern const char expiration_warning[];
extern const char expired[];
extern const char outzone[];
extern const char unguarded[];
extern const char outzone_unguarded_warning[];
extern const char outzone_unguarded[];
extern const char delete_candidate[];
extern const char delete_warning[];
extern const char validation_warning1[];
extern const char validation_warning2[];
extern const char not_validated[];

extern const char server_outzone_manual[];
extern const char server_inzone_manual[];
extern const char server_registrant_change_prohibited[];
extern const char server_renew_prohibited[];

extern const char server_blocked[];
extern const char server_delete_prohibited[];
extern const char server_transfer_prohibited[];
extern const char server_update_prohibited[];

} // namespace LibFred::RegistrableObject::Domain::StateFlagName

using NssetMissing = State::Flag::AutomaticInternal<StateFlagName::nsset_missing>;
using ExpirationWarning = State::Flag::AutomaticInternal<StateFlagName::expiration_warning>;
using Expired = State::Flag::AutomaticExternal<StateFlagName::expired>;
using Outzone = State::Flag::AutomaticExternal<StateFlagName::outzone>;
using Unguarded = State::Flag::AutomaticInternal<StateFlagName::unguarded>;
using OutzoneUnguardedWarning = State::Flag::AutomaticInternal<StateFlagName::outzone_unguarded_warning>;
using OutzoneUnguarded = State::Flag::AutomaticInternal<StateFlagName::outzone_unguarded>;
using DeleteCandidate = State::Flag::AutomaticExternal<StateFlagName::delete_candidate>;
using DeleteWarning = State::Flag::AutomaticInternal<StateFlagName::delete_warning>;
using ValidationWarning1 = State::Flag::AutomaticInternal<StateFlagName::validation_warning1>;
using ValidationWarning2 = State::Flag::AutomaticInternal<StateFlagName::validation_warning2>;
using NotValidated = State::Flag::AutomaticExternal<StateFlagName::not_validated>;

using ServerOutzoneManual = State::Flag::ManualExternal<StateFlagName::server_outzone_manual>;
using ServerInzoneManual = State::Flag::ManualExternal<StateFlagName::server_inzone_manual>;
using ServerRegistrantChangeProhibited = State::Flag::ManualExternal<StateFlagName::server_registrant_change_prohibited>;
using ServerRenewProhibited = State::Flag::ManualExternal<StateFlagName::server_renew_prohibited>;

using ServerBlocked = State::Flag::ManualExternal<StateFlagName::server_blocked>;
using ServerDeleteProhibited = State::Flag::ManualExternal<StateFlagName::server_delete_prohibited>;
using ServerTransferProhibited = State::Flag::ManualExternal<StateFlagName::server_transfer_prohibited>;
using ServerUpdateProhibited = State::Flag::ManualExternal<StateFlagName::server_update_prohibited>;

using DomainStateProvide = State::Of<Object_Type::domain,
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

using DomainState = typename DomainStateProvide::Type;

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
