/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#ifndef NSSET_STATE_HH_F6D26B3A06094F34BA4246C12CC9D615
#define NSSET_STATE_HH_F6D26B3A06094F34BA4246C12CC9D615

#include "libfred/registrable_object/state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Nsset {

namespace StateFlagName {

extern const char delete_candidate[];
extern const char linked[];

extern const char server_delete_prohibited[];
extern const char server_transfer_prohibited[];
extern const char server_update_prohibited[];

} // namespace LibFred::RegistrableObject::Nsset::StateFlagName

using DeleteCandidate = State::Flag::AutomaticExternal<StateFlagName::delete_candidate>;
using Linked = State::Flag::AutomaticExternal<StateFlagName::linked>;

using ServerDeleteProhibited = State::Flag::ManualExternal<StateFlagName::server_delete_prohibited>;
using ServerTransferProhibited = State::Flag::ManualExternal<StateFlagName::server_transfer_prohibited>;
using ServerUpdateProhibited = State::Flag::ManualExternal<StateFlagName::server_update_prohibited>;

using NssetStateProvide = State::Of<Object_Type::nsset,
        DeleteCandidate,
        Linked,
        ServerDeleteProhibited,
        ServerTransferProhibited,
        ServerUpdateProhibited>;

using NssetState = typename NssetStateProvide::Type;

} // namespace LibFred::RegistrableObject::Nsset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
