/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
/**
 *  @file
 */

#ifndef CHECK_CONTACT_HH_DE10D214C1FE4D5E888FBE3FFD666447
#define CHECK_CONTACT_HH_DE10D214C1FE4D5E888FBE3FFD666447

#include "libfred/registrable_object/contact/handle_state.hh"

#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace Contact {

ContactHandleState::SyntaxValidity::Enum get_handle_syntax_validity(OperationContext& _ctx, const std::string& _contact_handle);

ContactHandleState::Registrability::Enum get_handle_registrability(OperationContext& _ctx, const std::string& _contact_handle);

}//namespace LibFred::Contact
}//namespace LibFred

#endif
