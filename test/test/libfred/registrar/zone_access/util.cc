/*
 * Copyright (C) 2020-2022  CZ.NIC, z. s. p. o.
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
#include "test/libfred/registrar/zone_access/util.hh"

namespace Test {

unsigned long long get_zone_access_id(const ::LibFred::OperationContext& _ctx,
        const std::string& _registrar,
        const std::string& _zone,
        const boost::gregorian::date& _from_date,
        const boost::gregorian::date& _to_date)
{
    const Database::Result db_result = _ctx.get_conn().exec_params(
            // clang-format off
            "SELECT ri.id FROM registrarinvoice AS ri "
            "JOIN zone AS z ON z.id=ri.zone "
            "JOIN registrar AS r ON r.id=ri.registrarid "
            "WHERE r.handle = UPPER($1::text) "
            "AND z.fqdn = LOWER($2::text) "
            "AND ri.fromdate = $3::date "
            "AND COALESCE(NULLIF(ri.todate, $4::date), NULLIF($4::date, ri.todate)) IS NULL ",
            // clang-format on
            Database::query_param_list(_registrar)
                    (_zone)
                    (_from_date)
                    (_to_date.is_special() ? Database::QPNull : Database::QueryParam(_to_date)));

    if (db_result.size() != 1)
    {
        throw std::runtime_error("Failed to get new zone access id.");
    }
    const auto id = static_cast<unsigned long long>(db_result[0][0]);
    return id;
}

} // namespace Test
