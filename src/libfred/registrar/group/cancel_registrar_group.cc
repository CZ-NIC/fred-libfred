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
#include "libfred/registrar/group/cancel_registrar_group.hh"
#include "libfred/registrar/group/exceptions.hh"

namespace LibFred {
namespace Registrar {

void CancelRegistrarGroup::exec(const OperationContext& _ctx)
{
    try
    {
        const Database::Result already_cancelled = _ctx.get_conn().exec_params(
                "SELECT cancelled FROM registrar_group WHERE id=$1::bigint AND cancelled IS NOT NULL FOR UPDATE",
                Database::query_param_list(group_id_));
        if (already_cancelled.size() > 0)
        {
            throw AlreadyCancelled();
        }

        _ctx.get_conn().exec("LOCK TABLE registrar_group_map IN ACCESS EXCLUSIVE MODE");
        const Database::Result nonempty_group = _ctx.get_conn().exec_params(
                "SELECT id "
                "FROM registrar_group_map "
                "WHERE registrar_group_id = $1::bigint "
                "AND member_from <= CURRENT_DATE "
                "AND (member_until IS NULL "
                "OR (member_until >= CURRENT_DATE "
                "AND member_from <> member_until)) ",
                Database::query_param_list(group_id_));
        if (nonempty_group.size() > 0)
        {
            throw NonemptyGroupDelete();
        }

        _ctx.get_conn().exec_params(
                Database::ParamQuery(
                    "UPDATE registrar_group "
                    "SET cancelled=now() "
                    "WHERE id=")
                .param_bigint(group_id_));
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to cancel registrar group due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred
