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
#include "libfred/zone/zone_ns/create_zone_ns.hh"
#include "libfred/zone/zone_ns/exceptions.hh"
#include "libfred/zone/zone_ns/util.hh"
#include "libfred/zone/info_zone.hh"

#include <algorithm>
#include <sstream>

namespace LibFred {
namespace Zone {

CreateZoneNs::CreateZoneNs(const std::string& _zone_fqdn, const std::string& _nameserver_fqdn)
    : zone_fqdn_(_zone_fqdn),
      nameserver_fqdn_(_nameserver_fqdn)
{
}

CreateZoneNs& CreateZoneNs::set_nameserver_ip_addresses(
        const std::vector<boost::asio::ip::address>& _nameserver_ip_addresses)
{
    nameserver_ip_addresses_ = _nameserver_ip_addresses;
    return *this;
}

unsigned long long CreateZoneNs::exec(const OperationContext& _ctx) const
{
    const InfoZoneData zone_info = InfoZone(zone_fqdn_).exec(_ctx);
    const unsigned long long zone_id = get_zone_id(zone_info);

    try
    {
        const Database::Result insert_result = _ctx.get_conn().exec_params(
                // clang-format off
                "INSERT INTO zone_ns (zone, fqdn, addrs) "
                "VALUES ($1::bigint, LOWER($2::text), $3::inet[]) "
                "RETURNING id",
                // clang-format on
                Database::query_param_list(zone_id)
                                          (nameserver_fqdn_)
                                          (ip_addresses_to_string(nameserver_ip_addresses_)));

        if (insert_result.size() == 1)
        {
            const unsigned long long id = static_cast<unsigned long long>(insert_result[0][0]);
            return id;
        }
    }
    catch (const std::exception&)
    {
        throw CreateZoneNsException();
    }
    throw CreateZoneNsException();
}

} // namespace LibFred::Zone
} // namespace LibFred
