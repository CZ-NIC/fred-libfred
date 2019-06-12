/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/zone_access/exceptions.hh"
#include "libfred/registrar/zone_access/get_registrar_zone_access.hh"
#include "util/db/query_param.hh"

#include <sstream>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

GetZoneAccess::GetZoneAccess(const std::string& _registrar_handle)
        : registrar_handle_(_registrar_handle)
{
}

GetZoneAccess& GetZoneAccess::set_zone_fqdn(const std::string& _zone_fqdn)
{
    zone_fqdn_ = _zone_fqdn;
    return *this;
}

GetZoneAccess& GetZoneAccess::set_date(const boost::gregorian::date& _date)
{
    date_ = _date;
    return *this;
}

RegistrarZoneAccesses GetZoneAccess::exec(OperationContext& _ctx) const
{
    std::ostringstream object_sql;
    object_sql << "SELECT ri.id, z.fqdn AS zone, ri.fromdate, ri.todate "
               << "FROM registrar r "
               << "JOIN registrarinvoice ri ON r.id=ri.registrarid "
               << "JOIN zone z ON z.id=ri.zone ";

    Database::QueryParams params;
    params.push_back(registrar_handle_);
    object_sql << "WHERE r.handle=UPPER($" << params.size() << "::text) ";

    if (!zone_fqdn_.empty())
    {
        params.push_back(zone_fqdn_);
        object_sql << "AND z.fqdn = LOWER($" << params.size() << "::text) ";
    }

    if (!date_.is_special())
    {
        params.push_back(date_);
        object_sql << "AND ri.fromdate <= $" << params.size() << "::date ";
        object_sql << "AND (ri.todate IS NULL OR ri.todate >= $" << params.size() << "::date) ";
    }

    try
    {
        const Database::Result db_result = _ctx.get_conn().exec_params(object_sql.str(), params);

        RegistrarZoneAccesses zone_accesses;
        if (db_result.size() > 0)
        {
            for (std::size_t i = 0; i < db_result.size(); ++i)
            {
                ZoneAccess access;
                access.id = static_cast<unsigned long long>(db_result[i]["id"]);
                access.zone_fqdn = static_cast<std::string>(db_result[i]["zone"]);
                if (!db_result[i]["fromdate"].isnull())
                {
                    access.from_date = boost::gregorian::from_string(
                            static_cast<std::string>(db_result[i]["fromdate"]));
                }
                if (!db_result[i]["todate"].isnull())
                {
                    access.to_date = boost::gregorian::from_string(
                            static_cast<std::string>(db_result[i]["todate"]));
                }
                zone_accesses.zone_accesses.push_back(std::move(access));
            }
            zone_accesses.registrar_handle = registrar_handle_;
        }
        return zone_accesses;
    }
    catch (const std::exception&)
    {
        throw GetRegistrarZoneAccessException();
    }
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred
