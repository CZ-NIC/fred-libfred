/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#include "libfred/registrable_object/domain/info_domain_data.hh"
#include "libfred/registrable_object/domain/info_domain_diff.hh"
#include "util/util.hh"

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <string>

namespace LibFred {

InfoDomainData::InfoDomainData()
    : historyid(0),
      crhistoryid(0),
      id(0)
{}

bool InfoDomainData::operator==(const InfoDomainData& rhs) const
{
    return diff_domain_data(*this, rhs).is_empty();
}

bool InfoDomainData::operator!=(const InfoDomainData& rhs) const
{
    return !this->operator==(rhs);
}

std::string InfoDomainData::to_string() const
{
    return Util::format_data_structure(
            "InfoDomainData",
            {
                std::make_pair("roid", roid),
                std::make_pair("fqdn", fqdn),
                std::make_pair("registrant", registrant.to_string()),
                std::make_pair("nsset", nsset.print_quoted()),
                std::make_pair("keyset", keyset.print_quoted()),
                std::make_pair("sponsoring_registrar_handle", sponsoring_registrar_handle),
                std::make_pair("create_registrar_handle", create_registrar_handle),
                std::make_pair("update_registrar_handle", update_registrar_handle.print_quoted()),
                std::make_pair("creation_time", boost::lexical_cast<std::string>(creation_time)),
                std::make_pair("update_time", update_time.print_quoted()),
                std::make_pair("transfer_time", transfer_time.print_quoted()),
                std::make_pair("expiration_date", boost::lexical_cast<std::string>(expiration_date)),
                std::make_pair("admin_contacts", Util::format_container(admin_contacts)),
                std::make_pair("enum_domain_validation", enum_domain_validation.print_quoted()),
                std::make_pair("delete_time", delete_time.print_quoted()),
                std::make_pair("crhistoryid", std::to_string(crhistoryid)),
                std::make_pair("historyid", std::to_string(historyid)),
                std::make_pair("history_uuid", strong_to_string(history_uuid)),
                std::make_pair("id", std::to_string(id)),
                std::make_pair("uuid", strong_to_string(uuid)),
                std::make_pair("zone", zone.to_string())
            });
}

}//namespace LibFred
