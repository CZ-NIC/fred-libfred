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
#ifndef INFO_ZONE_NS_HH_A53EA0D75A694D84A4ADCD539AC6E3F8
#define INFO_ZONE_NS_HH_A53EA0D75A694D84A4ADCD539AC6E3F8

#include "libfred/opcontext.hh"
#include "libfred/zone/zone_ns/info_zone_ns_data.hh"

#include <string>

namespace LibFred {
namespace Zone {

class InfoZoneNs
{
public:
    explicit InfoZoneNs(unsigned long long _id);

    InfoZoneNsData exec(const OperationContext& _ctx) const;

private:
    unsigned long long id_;
};

} // namespace LibFred::Zone
} // namespace LibFred

#endif
