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
#include "libfred/zone/zone_soa/default_values.hh"
#include "libfred/zone/zone_soa/create_zone_soa.hh"
#include "libfred/zone/zone_soa/exceptions.hh"

namespace LibFred {
namespace Zone {

CreateZoneSoa::CreateZoneSoa(
        const std::string& _fqdn,
        const std::string& _hostmaster,
        const std::string& _ns_fqdn)
    : fqdn_(_fqdn),
      hostmaster_(_hostmaster),
      ns_fqdn_(_ns_fqdn)
{
}

CreateZoneSoa& CreateZoneSoa::set_ttl(const boost::optional<unsigned long>& _ttl)
{
    ttl_ = _ttl;
    return *this;
}

CreateZoneSoa& CreateZoneSoa::set_refresh(const boost::optional<unsigned long>& _refresh)
{
    refresh_ = _refresh;
    return *this;
}

CreateZoneSoa& CreateZoneSoa::set_update_retr(const boost::optional<unsigned long>& _update_retr)
{
    update_retr_ = _update_retr;
    return *this;
}

CreateZoneSoa& CreateZoneSoa::set_expiry(const boost::optional<unsigned long>& _expiry)
{
    expiry_ = _expiry;
    return *this;
}

CreateZoneSoa& CreateZoneSoa::set_minimum(const boost::optional<unsigned long>& _minimum)
{
    minimum_ = _minimum;
    return *this;
}

unsigned long long CreateZoneSoa::exec(const OperationContext& _ctx) const
{
    const LibFred::Zone::InfoZoneData zone_info = LibFred::Zone::InfoZone(fqdn_).exec(_ctx);

    const unsigned long long zone_id = LibFred::Zone::get_zone_id(zone_info);

    if (0 < _ctx.get_conn().exec_params(
            // clang-format off
            "SELECT 0 FROM zone_soa WHERE zone=$1::bigint",
            // clang-format on
            Database::query_param_list(zone_id)).size())
    {
        throw AlreadyExistingZoneSoa();
    }

    try
    {
        const Database::Result insert_result = _ctx.get_conn().exec_params(
                // clang-format off
                "INSERT INTO zone_soa (zone,ttl,hostmaster,refresh,update_retr,expiry,minimum,ns_fqdn) "
                "VALUES ($1::bigint,$2::integer,$3::text,$4::integer,$5::integer,$6::integer,$7::integer,$8::text) "
                "RETURNING zone",
                // clang-format on
                Database::query_param_list(zone_id)
                                          (ttl_.get_value_or(default_ttl_in_seconds))
                                          (hostmaster_)
                                          (refresh_.get_value_or(default_refresh_in_seconds))
                                          (update_retr_.get_value_or(default_update_retr_in_seconds))
                                          (expiry_.get_value_or(default_expiry_in_seconds))
                                          (minimum_.get_value_or(default_minimum_in_seconds))
                                          (ns_fqdn_));

        if (insert_result.size() == 1)
        {
            const auto id = static_cast<unsigned long long>(insert_result[0][0]);
            return id;
        }
    }
    catch (const std::exception&)
    {
        throw CreateZoneSoaException();
    }
    throw CreateZoneSoaException();
}

} // namespace LibFred::Zone
} // namespace LibFred
