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

#include "src/libfred/registrar/zone_access/registrar_zone_access_history.hh"

#include <cstring>

#include <stdexcept>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

bool RegistrarZoneAccessHistory::CompareByTime::operator()(const TimeInterval& lhs, const TimeInterval& rhs)const
{
    return (lhs.to_date != boost::none) && !rhs.from_date.is_special() && (*lhs.to_date < rhs.from_date);
}

bool RegistrarZoneAccessHistory::CompareCaseInsensitive::operator()(const std::string& lhs, const std::string& rhs)const
{
    return ::strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
}

bool has_access(
        const RegistrarZoneAccessHistory& zone_access,
        const std::string& zone_fqdn,
        const boost::gregorian::date& date)
{
    const auto& zone_itr = zone_access.invoices_by_zone.find(zone_fqdn);
    const bool zone_exists = zone_itr != zone_access.invoices_by_zone.end();
    if (zone_exists)
    {
        TimeInterval time_interval;
        time_interval.from_date = date;
        time_interval.to_date = date;
        const auto& invoice_itr = zone_itr->second.find(time_interval);
        const bool invoice_exists = invoice_itr != zone_itr->second.end();
        return invoice_exists;
    }
    return false;
}

}//namespace LibFred::Registrar::ZoneAccess
}//namespace LibFred::Registrar
}//namespace LibFred

namespace Database {

namespace {

template <typename T>
T make_strong_type_from_database_value(const Value& src)
{
    if (src.isnull())
    {
        struct UnexpectedNullValue : std::runtime_error
        {
            UnexpectedNullValue() : std::runtime_error("unable convert NULL database value to strong type") { }
        };
        throw UnexpectedNullValue();
    }
    return Util::make_strong<T>(src.as<typename T::UnderlyingType>());
}

}//namespace Database::{anonymous}

template <>
LibFred::Registrar::ZoneAccess::RegistrarInvoiceId Value::as()const
{
    return make_strong_type_from_database_value<LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(*this);
}

}//namespace Database
