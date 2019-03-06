/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/group/membership/end_registrar_group_membership.hh"
#include "libfred/registrar/group/membership/exceptions.hh"

#include "libfred/db_settings.hh"

namespace LibFred {
namespace Registrar {

void EndRegistrarGroupMembership::exec(OperationContext& _ctx)
{
    try
    {
        const Database::Result membership = _ctx.get_conn().exec_params(
                "SELECT id FROM registrar_group_map "
                "WHERE registrar_id=$1::bigint "
                "AND registrar_group_id=$2::bigint "
                "AND (member_until IS NULL OR member_until >= NOW())",
                Database::query_param_list(registrar_id_)(group_id_));
        if (membership.size() < 1)
        {
            throw MembershipNotFound();
        }
        else if (membership.size() > 1)
        {
            LOGGER.info("Failed to end registrar group membership because too many duplicate records were found.");
            throw;
        }

        _ctx.get_conn().exec_params(
            "UPDATE registrar_group_map SET member_until=NOW()::date WHERE id=$1::bigint",
            Database::query_param_list(static_cast<unsigned long long>(membership[0][0])));
    }
    catch (const std::exception& e)
    {
        LOGGER.error(e.what());
        throw;
    }
    catch (...)
    {
        LOGGER.info("Failed to end registrar group membership due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred

