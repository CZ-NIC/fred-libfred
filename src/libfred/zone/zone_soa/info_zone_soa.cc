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
#include "libfred/zone/info_zone.hh"
#include "libfred/zone/zone_soa/info_zone_soa.hh"
#include "libfred/zone/zone_soa/exceptions.hh"

namespace LibFred {
namespace Zone {

InfoZoneSoa::InfoZoneSoa(const std::string& _fqdn)
    : fqdn_(_fqdn)
{
}

InfoZoneSoaData InfoZoneSoa::exec(const OperationContext& _ctx) const
{
    const LibFred::Zone::InfoZoneData zone_info = LibFred::Zone::InfoZone(fqdn_).exec(_ctx);

    const unsigned long long zone_id = LibFred::Zone::get_zone_id(zone_info);

    Database::Result select_result;
    try
    {
        select_result = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT zone, ttl, hostmaster, refresh, update_retr, expiry, minimum, ns_fqdn "
                "FROM zone_soa "
                "WHERE zone = $1::bigint",
                // clang-format on
                Database::query_param_list(zone_id));
    }
    catch (const std::exception&)
    {
        throw InfoZoneSoaException();
    }

    if (select_result.size() == 0)
    {
        throw NonExistentZoneSoa();
    }

    InfoZoneSoaData info_zone_soa_data;

    info_zone_soa_data.zone = static_cast<unsigned long long>(select_result[0]["zone"]);
    info_zone_soa_data.ttl = static_cast<int>(select_result[0]["ttl"]);
    info_zone_soa_data.hostmaster = static_cast<std::string>(select_result[0]["hostmaster"]);
    info_zone_soa_data.refresh = static_cast<unsigned long long>(select_result[0]["refresh"]);
    info_zone_soa_data.update_retr = static_cast<unsigned long long>(select_result[0]["update_retr"]);
    info_zone_soa_data.expiry = static_cast<unsigned long long>(select_result[0]["expiry"]);
    info_zone_soa_data.minimum = static_cast<unsigned long long>(select_result[0]["minimum"]);
    info_zone_soa_data.ns_fqdn = static_cast<std::string>(select_result[0]["ns_fqdn"]);

    return info_zone_soa_data;
}

} // namespace LibFred::Zone
} // namespace LibFred
