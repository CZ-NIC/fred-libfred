/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#ifndef ADD_REGISTRAR_ZONE_ACCESS_HH_E55A42BCF78440A19288359591B3AB73
#define ADD_REGISTRAR_ZONE_ACCESS_HH_E55A42BCF78440A19288359591B3AB73

#include "libfred/opcontext.hh"

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/optional.hpp>

#include <string>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

class AddRegistrarZoneAccess{

public:
    AddRegistrarZoneAccess(
            const std::string& _registrar_handle,
            const std::string& _zone_fqdn,
            const boost::gregorian::date& _from_date);

    AddRegistrarZoneAccess& set_to_date(const boost::optional<boost::gregorian::date>& _to_date);

    unsigned long long exec(const OperationContext& _ctx) const;
private:
    std::string registrar_handle_;
    std::string zone_fqdn_;
    boost::gregorian::date from_date_;
    boost::optional<boost::gregorian::date> to_date_;
};

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
