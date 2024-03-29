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
#ifndef INFO_ZONE_HH_C7120B856BB24F0DBF1C2B9BCD170541
#define INFO_ZONE_HH_C7120B856BB24F0DBF1C2B9BCD170541

#include "libfred/opcontext.hh"
#include "libfred/zone/info_zone_data.hh"

#include <string>

namespace LibFred {
namespace Zone {

class InfoZone
{
public:
    explicit InfoZone(const std::string& _fqdn);

    InfoZoneData exec(const OperationContext& _ctx) const;
private:
    std::string fqdn_;
};

}//namespace LibFred::Zone
}//namespace LibFred

#endif//INFO_ZONE_HH_C7120B856BB24F0DBF1C2B9BCD170541
