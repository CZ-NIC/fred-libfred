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
/**
 *  @file
 *  nsset dns host
 */

#ifndef NSSET_DNS_HOST_HH_4D35DC3B927B44738A33B885BE34293B
#define NSSET_DNS_HOST_HH_4D35DC3B927B44738A33B885BE34293B

#include "util/printable.hh"

#include <boost/asio/ip/address.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
 * Nameserver data container.
 */
class DnsHost : public Util::Printable<DnsHost>
{
public:
    /**
     * Constructor initializing all attributes.
     * @param _fqdn sets nameserver name into @ref fqdn_ attribute
     * @param _inet_addr sets addresses of the nameserver into @ref inet_addr_ attribute.
     */
    DnsHost(const std::string& _fqdn, const std::vector<boost::asio::ip::address>& _inet_addr)
    : fqdn_(_fqdn)
    , inet_addr_(_inet_addr)
    {}

    /**
     * Nameserver name getter.
     * @return name of nameserver viz @ref fqdn_
     */
    std::string get_fqdn() const
    {
        return fqdn_;
    }

    /**
     * Nameserver addresses getter.
     * @return addresses of nameserver field viz @ref inet_addr_
     */
    std::vector<boost::asio::ip::address> get_inet_addr() const
    {
        return inet_addr_;
    }

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string()const
    {
        return Util::format_data_structure("DnsHost",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("fqdn", fqdn_))
        (std::make_pair("inet_addr", Util::format_container(inet_addr_)))
        );
    }
private:
    std::string fqdn_;/**< fully qualified name of the nameserver host*/
    std::vector<boost::asio::ip::address> inet_addr_;/**< list of IPv4 or IPv6 addresses of the nameserver host*/
};

}//namespace LibFred

#endif//NSSET_DNS_HOST_HH_4D35DC3B927B44738A33B885BE34293B
