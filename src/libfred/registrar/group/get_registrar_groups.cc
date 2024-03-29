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
#include "libfred/registrar/group/get_registrar_groups.hh"
#include "libfred/registrar/group/registrar_group_type.hh"

namespace LibFred {
namespace Registrar {

std::vector<RegistrarGroup> GetRegistrarGroups::exec(const OperationContext& _ctx)
{
    try
    {
        std::vector<RegistrarGroup> result;
        const Database::Result groups = _ctx.get_conn().exec(
                "SELECT id, short_name, cancelled FROM registrar_group "
                "ORDER BY cancelled, short_name");
        result.reserve(groups.size());
        for (Database::Result::Iterator it = groups.begin(); it != groups.end(); ++it)
        {
            Database::Row::Iterator col = (*it).begin();
            RegistrarGroup rg;
            rg.id = *col;
            rg.name = static_cast<std::string>(*(++col));
            rg.cancelled = *(++col);
            result.push_back(std::move(rg));
        }
        return result;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to get registrar group due to an unknown exception");
        throw;
    }
}

} // namespace Registrar
} // namespace LibFred
