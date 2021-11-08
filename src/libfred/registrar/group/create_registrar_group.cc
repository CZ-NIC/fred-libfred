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
#include "libfred/db_settings.hh"
#include "libfred/registrar/group/create_registrar_group.hh"
#include "libfred/registrar/group/exceptions.hh"

#include <string>

namespace LibFred {
namespace Registrar {

unsigned long long CreateRegistrarGroup::exec(const OperationContext& _ctx)
{
    try
    {
        if (group_name_.empty())
        {
            throw EmptyGroupName();
        }

        _ctx.get_conn().exec("LOCK TABLE registrar_group IN ACCESS EXCLUSIVE MODE");
        const Database::Result grp_exists = _ctx.get_conn().exec_params(
                "SELECT id FROM registrar_group WHERE short_name=$1::text ",
                Database::query_param_list(group_name_));
        if (grp_exists.size() > 0)
        {
            throw GroupExists();
        }

        return _ctx.get_conn().exec_params(
                Database::ParamQuery(
                    "INSERT INTO registrar_group (short_name) "
                    "VALUES (")
                .param_text(group_name_)
                (") returning id")
                )[0][0];
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to create a registrar group due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred
