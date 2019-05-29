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

#ifndef KEYSET_STATE_HISTORY_HH_66E0F2B7409F4ECDB1F0CA4510CC28C9
#define KEYSET_STATE_HISTORY_HH_66E0F2B7409F4ECDB1F0CA4510CC28C9

#include "libfred/registrable_object/state_history.hh"
#include "libfred/registrable_object/keyset/keyset_state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Keyset {

using KeysetStateHistory = StateHistory<KeysetState>;

} // namespace LibFred::RegistrableObject::Keyset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
