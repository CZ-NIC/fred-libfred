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
#include "libfred/zone/zone_soa/exceptions.hh"
#include "libfred/zone/zone_soa/info_zone_soa.hh"
#include "libfred/zone/zone_soa/update_zone_soa.hh"
#include "util/db/query_param.hh"
#include "util/util.hh"

#include <sstream>

namespace LibFred {
namespace Zone {

UpdateZoneSoa::UpdateZoneSoa(const std::string& _fqdn)
    : fqdn_(_fqdn)
{
}

UpdateZoneSoa& UpdateZoneSoa::set_ttl(const boost::optional<int> _ttl)
{
    ttl_ = _ttl;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_hostmaster(const boost::optional<std::string>& _hostmaster)
{
    hostmaster_ = _hostmaster;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_refresh(const boost::optional<int> _refresh)
{
    refresh_ = _refresh;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_update_retr(const boost::optional<int> _update_retr)
{
    update_retr_ = _update_retr;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_expiry(const boost::optional<int> _expiry)
{
    expiry_ = _expiry;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_minimum(const boost::optional<int> _minimum)
{
    minimum_ = _minimum;
    return *this;
}

UpdateZoneSoa& UpdateZoneSoa::set_ns_fqdn(const boost::optional<std::string>& _ns_fqdn)
{
    ns_fqdn_ = _ns_fqdn;
    return *this;
}

unsigned long long UpdateZoneSoa::exec(const OperationContext& _ctx) const
{
    const bool values_for_update_are_set =
            (ttl_ != boost::none) ||
            (hostmaster_ != boost::none) ||
            (refresh_ != boost::none) ||
            (update_retr_ != boost::none) ||
            (expiry_ != boost::none) ||
            (minimum_ != boost::none) ||
            (ns_fqdn_ != boost::none);

    if (!values_for_update_are_set)
    {
        throw NoZoneSoaData();
    }

    const LibFred::Zone::InfoZoneSoaData zone_info = LibFred::Zone::InfoZoneSoa(fqdn_).exec(_ctx);
    const unsigned long long zone_id = zone_info.zone;

    Database::QueryParams params;
    std::ostringstream object_sql;
    Util::HeadSeparator set_separator(" SET ", ",");

    object_sql << "UPDATE zone_soa";
    if (ttl_ != boost::none)
    {
        params.push_back(*ttl_);
        object_sql << set_separator.get() <<  "ttl = $" << params.size() << "::integer";
    }
    if (hostmaster_ != boost::none)
    {
        params.push_back(*hostmaster_);
        object_sql << set_separator.get() <<  "hostmaster = $" << params.size() << "::text";
    }
    if (refresh_ != boost::none)
    {
        params.push_back(*refresh_);
        object_sql << set_separator.get() <<  "refresh = $" << params.size() << "::integer";
    }
    if (update_retr_ != boost::none)
    {
        params.push_back(*update_retr_);
        object_sql << set_separator.get() <<  "update_retr = $" << params.size() << "::integer";
    }
    if (expiry_ != boost::none)
    {
        params.push_back(*expiry_);
        object_sql << set_separator.get() <<  "expiry = $" << params.size() << "::integer";
    }
    if (minimum_ != boost::none)
    {
        params.push_back(*minimum_);
        object_sql << set_separator.get() <<  "minimum = $" << params.size() << "::integer";
    }
    if (ns_fqdn_ != boost::none)
    {
        params.push_back(*ns_fqdn_);
        object_sql << set_separator.get() <<  "ns_fqdn = $" << params.size() << "::text";
    }

    params.push_back(zone_id);
    object_sql << " WHERE zone = $" << params.size() << "::bigint RETURNING zone";

    try
    {
        const Database::Result update_result = _ctx.get_conn().exec_params(
                object_sql.str(),
                params);
        if (update_result.size() == 1)
        {
            const unsigned long long id = static_cast<unsigned long long>(update_result[0][0]);
            return id;
        }
    }
    catch (const std::exception&)
    {
        throw UpdateZoneSoaException();
    }

    throw UpdateZoneSoaException();
}

} // namespace LibFred::Zone
} // namespace LibFred
