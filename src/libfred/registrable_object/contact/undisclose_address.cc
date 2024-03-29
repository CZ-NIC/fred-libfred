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
#include "libfred/registrable_object/contact/undisclose_address.hh"

#include "libfred/object/object_states_info.hh"
#include "libfred/poll/create_poll_message.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/contact/update_contact.hh"

#include <string>

namespace LibFred {
namespace Contact {

void undisclose_address(
        const LibFred::OperationContext& _ctx,
        unsigned long long _contact_id,
        const std::string& _registrar_handle)
{
    LibFred::UpdateContactById update_contact(_contact_id, _registrar_handle);
    const unsigned long long history_id = update_contact.set_discloseaddress(false).exec(_ctx); // #21767
    LibFred::Poll::CreateUpdateOperationPollMessage<LibFred::Object_Type::contact>().exec(_ctx, history_id);
}

namespace {

bool are_preconditions_met_for_async_undisclose_address(
        const LibFred::OperationContext& _ctx,
        unsigned long long _contact_id)
{
    {
        const LibFred::InfoContactData info_contact_data = LibFred::InfoContactById(_contact_id).exec(_ctx).info_contact_data;
        {
            const bool address_is_disclosed = info_contact_data.discloseaddress;
            if (!address_is_disclosed)
            {
                return false;
            }
        }
        {
            const bool address_can_be_undisclosed = info_contact_data.organization.get_value_or("").empty();
            if (!address_can_be_undisclosed)
            {
                return false;
            }
        }
    }
    const LibFred::ObjectStatesInfo states(LibFred::GetObjectStates(_contact_id).exec(_ctx));
    return states.presents(LibFred::Object_State::identified_contact) ||
           states.presents(LibFred::Object_State::validated_contact);
}

}//namespace LibFred::Contact::{anonymous}

void undisclose_address_async(
        unsigned long long _contact_id,
        const std::string& _registrar_handle)
{
    LibFred::OperationContextCreator ctx;

    if (are_preconditions_met_for_async_undisclose_address(ctx, _contact_id))
    {
        FREDLOG_INFO(boost::format("processing async undisclose address of contact %1%: "
                                         "preconditions met, undisclosing address") % _contact_id);
        undisclose_address(ctx, _contact_id, _registrar_handle);
    }
    else
    {
        FREDLOG_INFO(boost::format("processing async undisclose address of contact %1%: "
                                         "preconditions not met, doing nothing") % _contact_id);
    }

    ctx.commit_transaction();
}

}//namespace LibFred::Contact
}//namespace LibFred
