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

#include "libfred/registrable_object/keyset/transfer_keyset.hh"

#include "libfred/registrable_object/keyset/info_keyset.hh"
#include "libfred/registrable_object/keyset/copy_history_impl.hh"
#include "libfred/object/transfer_object.hh"

#include "libfred/exception.hh"

#include <algorithm>
#include <utility>

namespace LibFred {

namespace {

auto get_joined_contacts(
        OperationContext& _ctx,
        const unsigned long long _keyset_id)
{
    const auto keyset_data = [&]()
    {
        try
        {
            return LibFred::InfoKeysetById{_keyset_id}.set_lock().exec(_ctx).info_keyset_data;
        }
        catch (const LibFred::InfoKeysetById::Exception& e)
        {
            if (e.is_set_unknown_object_id())
            {
                throw UnknownKeysetId();
            }
            throw;
        }
    }();
    std::set<std::string> contacts = {};
    std::for_each(begin(keyset_data.tech_contacts), end(keyset_data.tech_contacts), [&](auto&& contact_ref)
    {
        contacts.insert(contact_ref.handle);
    });
    return contacts;
}

}//namespace LibFred::{anonymous}

TransferKeyset::TransferKeyset(
        const unsigned long long _keyset_id,
        std::string _new_registrar_handle,
        std::string _authinfopw,
        const Nullable<unsigned long long>& _logd_request_id)
    : keyset_id_{_keyset_id},
      new_registrar_handle_{std::move(_new_registrar_handle)},
      authinfopw_{std::move(_authinfopw)},
      logd_request_id_{_logd_request_id}
{ }

unsigned long long TransferKeyset::exec(OperationContext& _ctx) const
{
    const auto new_history_id = [&]()
    {
        try
        {
            return LibFred::transfer_object(
                    _ctx,
                    keyset_id_,
                    new_registrar_handle_,
                    authinfopw_,
                    get_joined_contacts(_ctx, keyset_id_),
                    logd_request_id_);
        }
        catch (const UnknownObjectId&)
        {
            throw UnknownKeysetId();
        }
    }();
    copy_keyset_data_to_keyset_history_impl(_ctx, keyset_id_, new_history_id);
    return new_history_id;
}

}//namespace LibFred
