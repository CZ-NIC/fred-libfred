/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#include "libfred/registrable_object/contact/transfer_contact.hh"

#include "libfred/registrable_object/contact/copy_history_impl.hh"
#include "libfred/object/transfer_object.hh"

#include "libfred/exception.hh"

#include <utility>

namespace LibFred {

TransferContact::TransferContact(
        const unsigned long long _contact_id,
        std::string _new_registrar_handle,
        std::string _authinfopw,
        const Nullable<unsigned long long>& _logd_request_id)
    : contact_id_{_contact_id},
      new_registrar_handle_{std::move(_new_registrar_handle)},
      authinfopw_{std::move(_authinfopw)},
      logd_request_id_{_logd_request_id}
{ }

unsigned long long TransferContact::exec(const OperationContext& _ctx) const
{
    const auto new_history_id = [&]()
    {
        try
        {
            return ::LibFred::transfer_object(
                    _ctx,
                    contact_id_,
                    new_registrar_handle_,
                    authinfopw_,
                    {},
                    logd_request_id_);
        }
        catch (const UnknownObjectId& e)
        {
            throw UnknownContactId{};
        }
    }();

    copy_contact_data_to_contact_history_impl(_ctx, contact_id_, new_history_id);

    return new_history_id;
}

}//namespace LibFred
