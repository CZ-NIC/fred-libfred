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
#ifndef ZONE_CREDIT_HH_0EC3DB4D6C6F48019FA034AA21B50A54
#define ZONE_CREDIT_HH_0EC3DB4D6C6F48019FA034AA21B50A54

#include "util/decimal/decimal.hh"

#include <string>
#include <boost/optional.hpp>

namespace LibFred {

class ZoneCredit
{
public:
    explicit ZoneCredit(const std::string& _zone_fqdn);
    ZoneCredit(const std::string& _zone_fqdn, const Decimal& _credit);
    const std::string& get_zone_fqdn()const;
    bool has_credit()const;
    const Decimal& get_credit()const;
private:
    const std::string zone_fqdn_;
    const boost::optional<Decimal> credit_;
};

} // namespace LibFred

#endif
