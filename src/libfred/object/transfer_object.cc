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

#include "libfred/object/transfer_object.hh"

#include "libfred/object/check_authinfo.hh"
#include "libfred/object/clean_authinfo.hh"
#include "libfred/object/object.hh"
#include "libfred/object/transfer_object_exception.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrar/info_registrar.hh"

#include <algorithm>

namespace LibFred {

namespace {

auto get_locked_sponsoring_registrar(
        const LibFred::OperationContext& _ctx,
        const Object::ObjectId& _object_id)
{
    const auto dbres = _ctx.get_conn().exec_params(
            "SELECT clid "
              "FROM object "
             "WHERE id = $1::integer "
               "FOR UPDATE",
            Database::QueryParams{_object_id});
    if (dbres.size() == 1)
    {
        return static_cast<unsigned long long>(dbres[0][0]);
    }
    if (dbres.size() < 1)
    {
        throw UnknownObjectId{};
    }
    throw std::runtime_error("something is really broken - nonunique record in object table");
}

void set_new_sponsoring_registrar(
        const LibFred::OperationContext& _ctx,
        const Object::ObjectId& _object_id,
        const unsigned long long _new_sponsoring_registrar_id)
{
    const auto dbres = _ctx.get_conn().exec_params(
        "UPDATE object "
           "SET trdate = now(), "
               "clid = $2::integer "
         "WHERE id = $1::integer",
        Database::QueryParams{_object_id, _new_sponsoring_registrar_id});

    if (dbres.rows_affected() != 1)
    {
        throw std::runtime_error("UPDATE object failed");
    }
}

auto set_history_id(
        const LibFred::OperationContext& _ctx,
        const Object::ObjectId& _object_id,
        unsigned long long _history_id)
{
    const auto dbres = _ctx.get_conn().exec_params(
            "UPDATE object_registry "
               "SET historyid = $2::bigint "
             "WHERE id = $1::integer",
            Database::QueryParams{_object_id, _history_id});

    if (dbres.rows_affected() != 1)
    {
        throw std::runtime_error("UPDATE object_registry failed");
    }
    return _history_id;
}

auto get_authorized_contacts(
        const LibFred::OperationContext& _ctx,
        const std::string& _authinfopw,
        const std::set<std::string>& _contacts)
{
    std::set<std::string> authorized_contacts;
    std::copy_if(
            begin(_contacts),
            end(_contacts),
            std::inserter(authorized_contacts, end(authorized_contacts)),
            [&](auto&& contact_handle)
            {
                try
                {
                    const auto contact_id = Object::ObjectId{InfoContactByHandle{contact_handle}
                                                    .exec(_ctx).info_contact_data.id};
                    return 0 < Object::CheckAuthinfo{Object::ObjectId{contact_id}}
                                        .exec(_ctx, _authinfopw, Object::CheckAuthinfo::increment_usage);
                }
                catch (const InfoContactByHandle::Exception& e)
                {
                    if (!e.is_set_unknown_contact_handle())
                    {
                        throw;
                    }
                }
                catch (const UnknownObjectId&)
                { }
                return false;
            });
    return authorized_contacts;
}

}//namespace LibFred::{anonymous}

unsigned long long transfer_object(
        const LibFred::OperationContext& _ctx,
        const unsigned long long _object_id,
        const std::string& _new_registrar_handle,
        const std::string& _authinfopw,
        const std::set<std::string>& _enabled_contacts,
        const Nullable<unsigned long long>& _logd_request_id)
{
    const auto new_registrar_id = [&]()
    {
        try
        {
            return LibFred::InfoRegistrarByHandle(_new_registrar_handle).exec(_ctx).info_registrar_data.id;

        }
        catch (const LibFred::InfoRegistrarByHandle::Exception& e)
        {
            if (e.is_set_unknown_registrar_handle())
            {
                throw UnknownRegistrar();
            }
            throw;
        }
    }();

    if (get_locked_sponsoring_registrar(_ctx, Object::ObjectId{_object_id}) == new_registrar_id)
    {
        throw NewRegistrarIsAlreadySponsoring{};
    }

    const auto authinfo_of_object_used = 0 < Object::CheckAuthinfo{Object::ObjectId{_object_id}}
            .exec(_ctx, _authinfopw, Object::CheckAuthinfo::increment_usage_and_cancel);
    const auto authinfo_of_friendly_contacts_used = !get_authorized_contacts(_ctx, _authinfopw, _enabled_contacts).empty();
    if (!(authinfo_of_object_used || authinfo_of_friendly_contacts_used))
    {
        throw IncorrectAuthInfoPw{};
    }
    if (!authinfo_of_object_used)
    {
        Object::CleanAuthinfo{Object::ObjectId{_object_id}}.exec(_ctx);
    }

    set_new_sponsoring_registrar(_ctx, Object::ObjectId{_object_id}, new_registrar_id);

    return set_history_id(
            _ctx,
            Object::ObjectId{_object_id},
            LibFred::InsertHistory(_logd_request_id, _object_id).exec(_ctx));
}

}//namespace LibFred
