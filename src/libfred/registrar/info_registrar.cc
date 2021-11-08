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
 *  registrar info
 */

#include <algorithm>
#include <string>

#include "libfred/registrar/info_registrar.hh"
#include "libfred/registrar/info_registrar_impl.hh"
#include "libfred/opcontext.hh"
#include "util/util.hh"

namespace LibFred
{

    InfoRegistrarByHandle::InfoRegistrarByHandle(const std::string& handle)
        : handle_(handle)
        , lock_(false)
    {}

    InfoRegistrarByHandle& InfoRegistrarByHandle::set_lock(bool lock)//set lock object_registry row for registrar
    {
        lock_ = lock;
        return *this;
    }

    InfoRegistrarOutput InfoRegistrarByHandle::exec(const OperationContext& ctx,
        const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoRegistrarOutput> registrar_res;

        try
        {
            registrar_res = InfoRegistrar()
                .set_inline_view_filter(Database::ParamQuery(InfoRegistrar::GetAlias::handle())(" = UPPER(").param_text(handle_)(")"))
                    .set_lock(lock_)
                    .exec(ctx, local_timestamp_pg_time_zone_name);

            if (registrar_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_registrar_handle(handle_));
            }

            if (registrar_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return registrar_res.at(0);
    }//InfoRegistrarByHandle::exec

    std::string InfoRegistrarByHandle::to_string() const
    {
        return Util::format_operation_state("InfoRegistrarByHandle",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("handle", handle_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoRegistrarById::InfoRegistrarById(unsigned long long id)
        : id_(id)
        , lock_(false)
    {}

    InfoRegistrarById& InfoRegistrarById::set_lock(bool lock)//set lock object_registry row for registrar
    {
        lock_ = lock;
        return *this;
    }

    InfoRegistrarOutput InfoRegistrarById::exec(const OperationContext& ctx,
        const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoRegistrarOutput> registrar_res;

        try
        {
            registrar_res = InfoRegistrar()
                .set_inline_view_filter(Database::ParamQuery(InfoRegistrar::GetAlias::id())(" = ").param_bigint(id_))
                    .set_lock(lock_)
                    .exec(ctx, local_timestamp_pg_time_zone_name);

            if (registrar_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_registrar_id(id_));
            }

            if (registrar_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return registrar_res.at(0);
    }//InfoRegistrarById::exec

    std::string InfoRegistrarById::to_string() const
    {
        return Util::format_operation_state("InfoRegistrarById",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("id", boost::lexical_cast<std::string>(id_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoRegistrarAllExceptSystem::InfoRegistrarAllExceptSystem()
        : lock_(false)
    {}

    InfoRegistrarAllExceptSystem& InfoRegistrarAllExceptSystem::set_lock(bool lock)
    {
        lock_ = lock;
        return *this;
    }

    std::vector<InfoRegistrarOutput> InfoRegistrarAllExceptSystem::exec(const OperationContext& ctx,
        const std::string& local_timestamp_pg_time_zone_name)
    {
            return InfoRegistrar()
                    .set_inline_view_filter(Database::ParamQuery(InfoRegistrar::GetAlias::system())(" = FALSE"))
                    .set_lock(lock_)
                    .exec(ctx, local_timestamp_pg_time_zone_name);
    }

    std::string InfoRegistrarAllExceptSystem::to_string() const
    {
        return Util::format_operation_state("InfoRegistrarAllExceptSystem",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }




} // namespace LibFred

