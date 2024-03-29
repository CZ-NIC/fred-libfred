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
/**
 *  @file
 *  registrar info data
 */

#include <algorithm>
#include <string>

#include <boost/lexical_cast.hpp>

#include "util/util.hh"
#include "libfred/registrar/info_registrar_data.hh"
#include "libfred/registrar/info_registrar_diff.hh"

namespace LibFred
{

    InfoRegistrarData::InfoRegistrarData()
    : vat_payer(false), id(0)
    {}

    bool InfoRegistrarData::operator==(const InfoRegistrarData& rhs) const
    {
        return diff_registrar_data(*this, rhs).is_empty();
    }

    bool InfoRegistrarData::operator!=(const InfoRegistrarData& rhs) const
    {
        return !this->operator ==(rhs);
    }

    std::string InfoRegistrarData::to_string() const
    {
        return Util::format_data_structure("InfoRegistrarData",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("handle", handle))
        (std::make_pair("name", name.print_quoted()))
        (std::make_pair("organization", organization.print_quoted()))
        (std::make_pair("street1", street1.print_quoted()))
        (std::make_pair("street2", street2.print_quoted()))
        (std::make_pair("street3", street3.print_quoted()))
        (std::make_pair("city", city.print_quoted()))
        (std::make_pair("stateorprovince", stateorprovince.print_quoted()))
        (std::make_pair("postalcode", postalcode.print_quoted()))
        (std::make_pair("country", country.print_quoted()))
        (std::make_pair("telephone", telephone.print_quoted()))
        (std::make_pair("fax", fax.print_quoted()))
        (std::make_pair("email", email.print_quoted()))
        (std::make_pair("url", url.print_quoted()))
        (std::make_pair("system", system.print_quoted()))
        (std::make_pair("ico", ico.print_quoted()))
        (std::make_pair("dic", dic.print_quoted()))
        (std::make_pair("variable_symbol", variable_symbol.print_quoted()))
        (std::make_pair("payment_memo_regex", payment_memo_regex.print_quoted()))
        (std::make_pair("vat_payer", boost::lexical_cast<std::string>(vat_payer)))
        (std::make_pair("id", boost::lexical_cast<std::string>(id)))
        (std::make_pair("is_internal", boost::lexical_cast<std::string>(is_internal)))
        );
    }


} // namespace LibFred


