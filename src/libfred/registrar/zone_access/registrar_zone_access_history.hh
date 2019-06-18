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

#ifndef REGISTRAR_ZONE_ACCESS_HISTORY_HH_5B24C1B228B8CC2C47D615CB3A7E8B0E//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define REGISTRAR_ZONE_ACCESS_HISTORY_HH_5B24C1B228B8CC2C47D615CB3A7E8B0E

#include "util/strong_type.hh"
#include "util/db/value.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>

#include <string>
#include <map>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

using RegistrarInvoiceId = Util::StrongType<unsigned long long, struct RegistrarInvoiceId_Tag>;

struct TimeInterval
{
    boost::gregorian::date from_date;
    boost::optional<boost::gregorian::date> to_date;
};

struct RegistrarZoneAccessHistory
{
    std::string registrar_handle;
    struct CompareByTime
    {
        bool operator()(const TimeInterval& lhs, const TimeInterval& rhs)const;
    };
    using InvoiceIdByDate = std::map<TimeInterval, RegistrarInvoiceId, CompareByTime>;
    struct CompareCaseInsensitive
    {
        bool operator()(const std::string& lhs, const std::string& rhs)const;
    };
    using InvoicesByZone = std::map<std::string, InvoiceIdByDate, CompareCaseInsensitive>;
    InvoicesByZone invoices_by_zone;
};

bool has_access(
        const RegistrarZoneAccessHistory& zone_access,
        const std::string& zone_fqdn,
        const boost::gregorian::date& date);

}//namespace LibFred::Registrar::ZoneAccess
}//namespace LibFred::Registrar
}//namespace LibFred

namespace Database {

template <>
LibFred::Registrar::ZoneAccess::RegistrarInvoiceId Value::as()const;

}//namespace Database

#endif//REGISTRAR_ZONE_ACCESS_HISTORY_HH_5B24C1B228B8CC2C47D615CB3A7E8B0E
