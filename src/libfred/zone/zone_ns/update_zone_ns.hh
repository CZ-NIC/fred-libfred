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
#ifndef UPDATE_ZONE_NS_HH_4158B94CC93249B1BB4786C9F50A8742
#define UPDATE_ZONE_NS_HH_4158B94CC93249B1BB4786C9F50A8742

#include "libfred/opcontext.hh"

#include <boost/asio/ip/address.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace LibFred {
namespace Zone {

class UpdateZoneNs
{
public:
    explicit UpdateZoneNs(unsigned long long _id);

    UpdateZoneNs& set_zone_fqdn(const boost::optional<std::string>& _zone_fqdn);

    UpdateZoneNs& set_nameserver_fqdn(const boost::optional<std::string>& _nameserver_fqdn);

    UpdateZoneNs& set_nameserver_ip_addresses(
            const std::vector<boost::asio::ip::address>& _nameserver_ip_addresses);

    unsigned long long exec(const OperationContext& _ctx) const;

private:
    unsigned long long id_;
    boost::optional<std::string> zone_fqdn_;
    boost::optional<std::string> nameserver_fqdn_;
    boost::optional<std::vector<boost::asio::ip::address> > nameserver_ip_addresses_;
};

} // namespace LibFred::Zone
} // namespace LibFred

#endif
