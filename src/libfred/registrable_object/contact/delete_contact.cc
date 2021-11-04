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
/**
 *  @file
 *  contact delete
 */

#include "libfred/registrable_object/contact/delete_contact.hh"
#include "libfred/object/object.hh"
#include "libfred/object/object_impl.hh"
#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"
#include "libfred/object_state/object_has_state.hh"

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include <string>

namespace LibFred {

namespace {

void delete_contact_impl(const OperationContext& _ctx, unsigned long long _id)
{
    const Database::Result delete_contact_res = _ctx.get_conn().exec_params(
        "WITH detach_identity AS ("
            "UPDATE contact_identity "
            "SET valid_to = NOW() "
            "WHERE contact_id = $1::BIGINT AND "
                  "valid_to IS NULL), "
        "delete_address AS ("
            "DELETE FROM contact_address "
            "WHERE contactid = $1::BIGINT) "
        "DELETE FROM contact "
        "WHERE id = $1::BIGINT "
        "RETURNING id",
        Database::query_param_list{_id});

    if (delete_contact_res.size() != 1)
    {
        BOOST_THROW_EXCEPTION(LibFred::InternalError("delete contact failed"));
    }
}

}//namespace LibFred::{anonymous}

DeleteContactByHandle::DeleteContactByHandle(const std::string& handle)
: handle_(handle)
{}

void DeleteContactByHandle::exec(OperationContext& _ctx)
{
    try
    {
        unsigned long long contact_id = get_object_id_by_handle_and_type_with_lock(
            _ctx,
            true,
            handle_,
            "contact",
            static_cast<Exception*>(NULL),
            &Exception::set_unknown_contact_handle);

        if (ObjectHasState(contact_id, Object_State::linked).exec(_ctx))
        {
            BOOST_THROW_EXCEPTION(Exception().set_object_linked_to_contact_handle(handle_));
        }

        delete_contact_impl(_ctx, contact_id);

        LibFred::DeleteObjectByHandle(handle_, "contact").exec(_ctx);

    } catch(ExceptionStack& ex) {
        ex.add_exception_stack_info(to_string());
        throw;
    }

}

std::string DeleteContactByHandle::to_string() const
{
    return Util::format_operation_state(
        "DeleteContactByHandle",
        boost::assign::list_of
            (std::make_pair("handle", handle_ ))
    );
}


DeleteContactById::DeleteContactById(unsigned long long _id)
    : id_(_id)
{ }

void DeleteContactById::exec(OperationContext& _ctx)
{
    try
    {
        get_object_id_by_object_id_with_lock(
            _ctx,
            id_,
            static_cast<Exception*>(NULL),
            &Exception::set_unknown_contact_id
        );

        if (ObjectHasState(id_, Object_State::linked).exec(_ctx)) {
            BOOST_THROW_EXCEPTION(Exception().set_object_linked_to_contact_id(id_));
        }

        delete_contact_impl(_ctx, id_);

        LibFred::DeleteObjectById(id_).exec(_ctx);

    } catch(ExceptionStack& ex) {
        ex.add_exception_stack_info(to_string());
        throw;
    }

}

std::string DeleteContactById::to_string() const
{
    return Util::format_operation_state(
        "DeleteContactById",
        boost::assign::list_of
            (std::make_pair("id", boost::lexical_cast<std::string>(id_) ))
    );
}

}//namespace LibFred
