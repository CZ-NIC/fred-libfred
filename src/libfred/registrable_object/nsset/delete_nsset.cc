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
 *  nsset delete
 */

#include <string>
#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>

#include "libfred/registrable_object/nsset/delete_nsset.hh"
#include "libfred/object/object.hh"
#include "libfred/object/object_impl.hh"
#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"
#include "libfred/object_state/object_has_state.hh"

namespace LibFred
{
    static void delete_nsset_impl(const OperationContext& ctx, unsigned long long id) {
        ctx.get_conn().exec_params(
            "DELETE FROM nsset_contact_map "
            "   WHERE nssetid = $1::integer",
            Database::query_param_list(id));

        ctx.get_conn().exec_params(
            "DELETE FROM host_ipaddr_map "
            "   WHERE nssetid = $1::integer",
            Database::query_param_list(id));

        ctx.get_conn().exec_params(
            "DELETE FROM host "
            "   WHERE nssetid = $1::integer",
            Database::query_param_list(id));

        const Database::Result delete_nsset_res = ctx.get_conn().exec_params(
            "DELETE FROM nsset "
            "   WHERE id = $1::integer RETURNING id",
            Database::query_param_list(id));
        if (delete_nsset_res.size() != 1) {
            BOOST_THROW_EXCEPTION(LibFred::InternalError("delete nsset failed"));
        }
    }

    DeleteNssetByHandle::DeleteNssetByHandle(const std::string& handle)
    : handle_(handle)
    {}

    void DeleteNssetByHandle::exec(const OperationContext& _ctx)
    {
        try
        {
            unsigned long long nsset_id = get_object_id_by_handle_and_type_with_lock(
                _ctx,
                true,
                handle_,
                "nsset",
                static_cast<Exception*>(NULL),
                &Exception::set_unknown_nsset_handle);

            if (ObjectHasState(nsset_id, Object_State::linked).exec(_ctx)) {
                BOOST_THROW_EXCEPTION(Exception().set_object_linked_to_nsset_handle(handle_));
            }

            delete_nsset_impl(_ctx, nsset_id);

            LibFred::DeleteObjectByHandle(handle_, "nsset").exec(_ctx);

        } catch(ExceptionStack& ex) {
            ex.add_exception_stack_info(to_string());
            throw;
        }
    }

    std::string DeleteNssetByHandle::to_string() const
    {
        return Util::format_operation_state(
            "DeleteNssetByHandle",
            boost::assign::list_of
              (std::make_pair("handle", handle_ ))
        );
    }

    DeleteNssetById::DeleteNssetById(unsigned long long id)
    : id_(id)
    {}

    void DeleteNssetById::exec(const OperationContext& _ctx)
    {
        try
        {
            get_object_id_by_object_id_with_lock(
                _ctx,
                id_,
                static_cast<Exception*>(NULL),
                &Exception::set_unknown_nsset_id);

            if (ObjectHasState(id_, Object_State::linked).exec(_ctx)) {
                BOOST_THROW_EXCEPTION(Exception().set_object_linked_to_nsset_id(id_));
            }

            delete_nsset_impl(_ctx, id_);

            LibFred::DeleteObjectById(id_).exec(_ctx);

        } catch(ExceptionStack& ex) {
            ex.add_exception_stack_info(to_string());
            throw;
        }
    }

    std::string DeleteNssetById::to_string() const
    {
        return Util::format_operation_state(
            "DeleteNssetById",
            boost::assign::list_of
              (std::make_pair("id", boost::lexical_cast<std::string>(id_) ))
        );
    }
} // namespace LibFred

