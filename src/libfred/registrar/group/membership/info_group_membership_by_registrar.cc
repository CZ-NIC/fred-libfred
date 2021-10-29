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
#include "libfred/registrar/group/membership/info_group_membership_by_registrar.hh"
#include "libfred/db_settings.hh"

namespace LibFred {
namespace Registrar {

std::vector<GroupMembershipByRegistrar> InfoGroupMembershipByRegistrar::exec(OperationContext& _ctx)
{
    try
    {
        const Database::Result membership = _ctx.get_conn().exec_params(
                "SELECT id, registrar_group_id, member_from, member_until "
                "FROM registrar_group_map "
                "WHERE registrar_id=$1::bigint "
                "ORDER BY member_from DESC, id DESC",
                Database::query_param_list(registrar_id_));
        std::vector<GroupMembershipByRegistrar> result;
        result.reserve(membership.size());
        for (Database::Result::Iterator it = membership.begin(); it != membership.end(); ++it)
        {
            GroupMembershipByRegistrar tmp;
            tmp.membership_id = (*it)["id"];
            tmp.group_id = (*it)["registrar_group_id"];
            tmp.member_from = (*it)["member_from"];
            if (static_cast<std::string>((*it)["member_until"]).empty())
            {
                tmp.member_until = boost::gregorian::date(pos_infin);
            }
            else
            {
                tmp.member_until = (*it)["member_until"];
            }
            result.push_back(tmp);
        }
        return result;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to get info group membership by registrar due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred
