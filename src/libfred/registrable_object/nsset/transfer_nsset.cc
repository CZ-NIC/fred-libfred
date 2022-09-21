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

#include "libfred/registrable_object/nsset/transfer_nsset.hh"

#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/copy_history_impl.hh"
#include "libfred/object/transfer_object.hh"

#include "libfred/exception.hh"

#include <algorithm>
#include <utility>

namespace LibFred {

namespace {

auto get_joined_contacts(
        const OperationContext& _ctx,
        unsigned long long _nsset_id)
{
    const auto nsset_data = [&]()
    {
        try
        {
            return LibFred::InfoNssetById{_nsset_id}.set_lock().exec(_ctx).info_nsset_data;
        }
        catch (const LibFred::InfoNssetById::Exception& e)
        {
            if (e.is_set_unknown_object_id())
            {
                throw UnknownNssetId();
            }
            throw;
        }
    }();
    std::set<std::string> contacts = {};
    std::for_each(begin(nsset_data.tech_contacts), end(nsset_data.tech_contacts), [&](auto&& contact_ref)
    {
        contacts.insert(contact_ref.handle);
    });
    return contacts;
}

}//namespace LibFred::{anonymous}

TransferNsset::TransferNsset(
        const unsigned long long _nsset_id,
        std::string _new_registrar_handle,
        std::string _authinfopw,
        const Nullable<unsigned long long>& _logd_request_id)
    : nsset_id_{_nsset_id},
      new_registrar_handle_{std::move(_new_registrar_handle)},
      authinfopw_{std::move(_authinfopw)},
      logd_request_id_{_logd_request_id}
{ }

unsigned long long TransferNsset::exec(const OperationContext& _ctx) const
{
    const auto new_history_id = [&]()
    {
        try
        {
            return LibFred::transfer_object(
                    _ctx,
                    nsset_id_,
                    new_registrar_handle_,
                    authinfopw_,
                    get_joined_contacts(_ctx, nsset_id_),
                    logd_request_id_);
        }
        catch (const UnknownObjectId&)
        {
            throw UnknownNssetId();
        }
    }();
    copy_nsset_data_to_nsset_history_impl(_ctx, nsset_id_, new_history_id);
    return new_history_id;
}

}//namespace LibFred
