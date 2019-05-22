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

#include "libfred/registrable_object/keyset/keyset_state.hh"

#include "util/flagset_impl.hh"

namespace Libfred {
namespace RegistrableObject {
namespace Keyset {

namespace StateFlagName {

constexpr const char delete_candidate[] = "deleteCandidate";
constexpr const char linked[] = "linked";

constexpr const char server_delete_prohibited[] = "serverDeleteProhibited";
constexpr const char server_transfer_prohibited[] = "serverTransferProhibited";
constexpr const char server_update_prohibited[] = "serverUpdateProhibited";

} // namespace LibFred::RegistrableObject::Keyset::StateFlagName

} // namespace LibFred::RegistrableObject::Keyset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

namespace Util {

using namespace LibFred::RegistrableObject::Keyset;

template class FlagSet<KeysetStateProvide,
        DeleteCandidate,
        Linked,
        ServerDeleteProhibited,
        ServerTransferProhibited,
        ServerUpdateProhibited>;

} // namespace Util
