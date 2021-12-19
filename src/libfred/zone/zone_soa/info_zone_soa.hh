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
#ifndef INFO_ZONE_SOA_HH_D0F44B04C60244D3B7B7DE7E261CDADE
#define INFO_ZONE_SOA_HH_D0F44B04C60244D3B7B7DE7E261CDADE

#include "libfred/opcontext.hh"
#include "libfred/zone/zone_soa/info_zone_soa_data.hh"

#include <string>

namespace LibFred {
namespace Zone {

class InfoZoneSoa
{
public:
    explicit InfoZoneSoa(const std::string& _fqdn);

    InfoZoneSoaData exec(const OperationContext& _ctx) const;

private:
    std::string fqdn_;
};

} // namespace LibFred::Zone
} // namespace LibFred


#endif
