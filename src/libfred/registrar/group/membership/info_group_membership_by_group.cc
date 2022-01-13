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
#include "libfred/registrar/group/membership/info_group_membership_by_group.hh"
#include "libfred/db_settings.hh"

namespace LibFred {
namespace Registrar {

std::vector<GroupMembershipByGroup> InfoGroupMembershipByGroup::exec(const OperationContext& _ctx)
{
    try
    {
        const Database::Result membership = _ctx.get_conn().exec_params(
                "SELECT id, registrar_id, member_from, member_until "
                "FROM registrar_group_map "
                "WHERE registrar_group_id=$1::bigint "
                "ORDER BY member_from DESC, id DESC",
                Database::query_param_list(group_id_));
        std::vector<GroupMembershipByGroup> result;
        result.reserve(membership.size());
        for (Database::Result::Iterator it = membership.begin(); it != membership.end(); ++it)
        {
            GroupMembershipByGroup tmp;
            tmp.membership_id = (*it)["id"];
            tmp.registrar_id = (*it)["registrar_id"];
            tmp.member_from = (*it)["member_from"];
            if (static_cast<std::string>((*it)["member_until"]).empty())
            {
                tmp.member_until = boost::gregorian::date(pos_infin);
            }
            else
            {
                tmp.member_until = (*it)["member_until"];
            }
            result.push_back(std::move(tmp));
        }
        return result;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to get info group membership by group due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred
