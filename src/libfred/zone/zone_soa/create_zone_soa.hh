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
#ifndef CREATE_ZONE_SOA_HH_2816933E658E4D0CB9A4326C1DDA364E
#define CREATE_ZONE_SOA_HH_2816933E658E4D0CB9A4326C1DDA364E

#include "libfred/opcontext.hh"

#include <boost/optional.hpp>
#include <string>

namespace LibFred {
namespace Zone {

class CreateZoneSoa
{
public:
    CreateZoneSoa(const std::string& fqdn, const std::string& hostmaster, const std::string& ns_fqdn);

    CreateZoneSoa& set_ttl(const boost::optional<unsigned long>& ttl);
    CreateZoneSoa& set_refresh(const boost::optional<unsigned long>& refresh);
    CreateZoneSoa& set_update_retr(const boost::optional<unsigned long>& update_retr);
    CreateZoneSoa& set_expiry(const boost::optional<unsigned long>& expiry);
    CreateZoneSoa& set_minimum(const boost::optional<unsigned long>& minimum);

    unsigned long long exec(const OperationContext& ctx) const;
private:
    std::string fqdn_;
    std::string hostmaster_;
    std::string ns_fqdn_;
    boost::optional<unsigned long> ttl_;
    boost::optional<unsigned long> refresh_;
    boost::optional<unsigned long> update_retr_;
    boost::optional<unsigned long> expiry_;
    boost::optional<unsigned long> minimum_;
};

} // namespace LibFred::Zone
} // namespace LibFred

#endif
