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
#ifndef GET_REGISTRAR_ZONE_ACCESS_HH_D244602CE4E742B08CA40AE73028E1EF
#define GET_REGISTRAR_ZONE_ACCESS_HH_D244602CE4E742B08CA40AE73028E1EF

#include "libfred/opcontext.hh"
#include "libfred/registrar/zone_access/registrar_zone_access_type.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

class GetZoneAccess {
public:
    explicit GetZoneAccess(const std::string& _registrar_handle);

    GetZoneAccess& set_zone_fqdn(const std::string& _zone_fqdn);

    GetZoneAccess& set_date(const boost::gregorian::date& _date);

    RegistrarZoneAccesses exec(OperationContext& _ctx) const;

private:
    std::string registrar_handle_;
    std::string zone_fqdn_;
    boost::gregorian::date date_;
};

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
