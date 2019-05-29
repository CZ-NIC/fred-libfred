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

#ifndef KEYSET_STATE_HH_F0ADE54C04AF4C55B80A9197F87FEE18
#define KEYSET_STATE_HH_F0ADE54C04AF4C55B80A9197F87FEE18

#include "libfred/registrable_object/state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Keyset {

namespace StateFlagName {

extern const char delete_candidate[];
extern const char linked[];

extern const char server_delete_prohibited[];
extern const char server_transfer_prohibited[];
extern const char server_update_prohibited[];

} // namespace LibFred::RegistrableObject::Keyset::StateFlagName

using DeleteCandidate = State::Flag::AutomaticExternal<StateFlagName::delete_candidate>;
using Linked = State::Flag::AutomaticExternal<StateFlagName::linked>;

using ServerDeleteProhibited = State::Flag::ManualExternal<StateFlagName::server_delete_prohibited>;
using ServerTransferProhibited = State::Flag::ManualExternal<StateFlagName::server_transfer_prohibited>;
using ServerUpdateProhibited = State::Flag::ManualExternal<StateFlagName::server_update_prohibited>;

using KeysetStateProvide = State::Of<Object_Type::keyset,
        DeleteCandidate,
        Linked,
        ServerDeleteProhibited,
        ServerTransferProhibited,
        ServerUpdateProhibited>;

using KeysetState = typename KeysetStateProvide::Type;

} // namespace LibFred::RegistrableObject::Keyset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
