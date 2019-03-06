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
#include "libfred/zone/info_zone_data.hh"

#include <stdexcept>

namespace LibFred {
namespace Zone {

namespace {

class GetZoneId : public boost::static_visitor<unsigned long long>
{
public:
    unsigned long long operator()(const EnumZone& _zone) const
    {
            return _zone.id;
    }

    unsigned long long operator()(const NonEnumZone& _zone) const
    {
            return _zone.id;
    }

    unsigned long long operator()(const boost::blank&) const
    {
            throw std::runtime_error("Get zone id failed");
    }
};

} // namespace LibFred::Zone::{anonymous}

unsigned long long get_zone_id(const InfoZoneData& _zone)
{
    return boost::apply_visitor(GetZoneId(), _zone);
}

} // namespace LibFred::Zone
} // namespace LibFred
