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
#ifndef CREATE_ZONE_HH_3D464D2FBF714314991D5DF2D86C15F6
#define CREATE_ZONE_HH_3D464D2FBF714314991D5DF2D86C15F6

#include "libfred/opcontext.hh"

#include <boost/optional.hpp>
#include <string>


namespace LibFred {
namespace Zone {

class CreateZone
{
public:
    CreateZone(
            const std::string& _fqdn,
            int _expiration_period_min_in_months,
            int _expiration_period_max_in_months);

    CreateZone& set_enum_validation_period_in_months(boost::optional<int> _enum_validation_period_in_months);

    CreateZone& set_sending_warning_letter(boost::optional<bool> _sending_warning_letter);

    unsigned long long exec(const OperationContext& _ctx) const;

private:
    std::string fqdn_;
    int expiration_period_min_in_months_;
    int expiration_period_max_in_months_;
    boost::optional<bool> sending_warning_letter_;
    boost::optional<int> enum_validation_period_in_months_;
};

} // namespace LibFred::Zone
} // namespace LibFred

#endif
