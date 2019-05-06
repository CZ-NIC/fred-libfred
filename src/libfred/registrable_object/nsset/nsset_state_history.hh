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

#ifndef NSSET_STATE_HISTORY_HH_1C0297BC29EC47E3AE85D59BB7824209
#define NSSET_STATE_HISTORY_HH_1C0297BC29EC47E3AE85D59BB7824209

#include "libfred/registrable_object/state_history.hh"
#include "libfred/registrable_object/nsset/nsset_state.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Nsset {

using NssetStateHistory = StateHistory<NssetState>;

} // namespace LibFred::RegistrableObject::Nsset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
