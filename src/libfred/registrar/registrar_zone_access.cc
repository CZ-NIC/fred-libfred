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
#include "libfred/opcontext.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/registrar/info_registrar_data.hh"
#include "libfred/registrar/registrar_zone_access.hh"
#include "util/db/param_query_composition.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {

bool is_zone_accessible_by_registrar(unsigned long long _registrar_id,
        unsigned long long _zone_id,
        boost::gregorian::date _local_today,
        OperationContext& _ctx)
{
    const bool is_registrar_system = LibFred::InfoRegistrarById(_registrar_id).exec(_ctx).info_registrar_data.system.get_value_or(false);

    if (is_registrar_system)
    {
        return true;
    }

    Database::ReusableParameter local_today = Database::ReusableParameter(_local_today, "date");
    return _ctx.get_conn().exec_params(Database::ParamQuery(
                                               "SELECT id "
                                               "FROM registrarinvoice "
                                               "WHERE registrarid=")
                                               .param_bigint(_registrar_id)(" AND zone=")
                                               .param_bigint(_zone_id)(" AND fromdate <= ")
                                               .param(local_today)(" AND (todate >= ")
                                               .param(local_today)(" OR todate IS NULL)"))
                                        .size();
}

} // namespace LibFred
