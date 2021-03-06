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
#ifndef INFO_ZONE_DATA_HH_D239F816DDA648F1B5728C1B1767F185
#define INFO_ZONE_DATA_HH_D239F816DDA648F1B5728C1B1767F185

#include <boost/variant.hpp>
#include <string>

namespace LibFred {
namespace Zone {

struct NonEnumZone
{
    unsigned long long id;
    std::string fqdn;
    int expiration_period_min_in_months;
    int expiration_period_max_in_months;
    int dots_max;
    bool sending_warning_letter;
};

struct EnumZone
{
    unsigned long long id;
    std::string fqdn;
    int expiration_period_min_in_months;
    int expiration_period_max_in_months;
    int validation_period_in_months;
    int dots_max;
    bool sending_warning_letter;
};

using InfoZoneData = boost::variant<boost::blank, NonEnumZone, EnumZone>;

unsigned long long get_zone_id(const InfoZoneData& _zone);

} // namespace LibFred::Zone
} // namespace LibFred

#endif
