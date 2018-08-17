/*
 * Copyright (C) 2013  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file
 *  templates for contact enums
 */

#ifndef CONTACT_ENUM_HH_54DADDB16CD34A4BAABBA444F473BE6B
#define CONTACT_ENUM_HH_54DADDB16CD34A4BAABBA444F473BE6B

#include "src/libfred/opexception.hh"
#include "src/libfred/opcontext.hh"
#include "src/util/optional_value.hh"

#include <string>

namespace LibFred {
namespace Contact {

/**
* Gets "type of identification" database id.
* @param ssntype is "type of identification" to get
* @param ctx contains reference to database and logging interface
* @param ex_ptr is pointer to exception instance to set in case of failure,
* if nullptr, new exception instance is created, set and thrown in case of failure,
* if not nullptr, refered exception instance is only set
* @param ex_setter is exception member pointer used to set data into exception in case of failure
* @return "type of identification" database id or 0 indicating "no id found"
*/
template <class EXCEPTION, typename EXCEPTION_SETTER>
unsigned long long get_ssntype_id(
        const Optional<std::string>& ssntype,
        OperationContext& ctx,
        EXCEPTION* ex_ptr,
        EXCEPTION_SETTER ex_setter)
{
    constexpr unsigned long long invalid_ssntype_id = 0;
    if (ssntype.isset())
    {
        const Database::Result ssntype_res = ctx.get_conn().exec_params(
                "SELECT id FROM enum_ssntype WHERE UPPER(type)=UPPER($1::text)",
                Database::query_param_list(ssntype.get_value()));
        if (ssntype_res.size() == 1)
        {
            return static_cast<unsigned long long>(ssntype_res[0][0]);
        }
        if (ssntype_res.size() == 0)
        {
            if (ex_ptr == nullptr)
            {
                BOOST_THROW_EXCEPTION((EXCEPTION().*ex_setter)(ssntype.get_value()));
            }
            (ex_ptr->*ex_setter)(ssntype.get_value());
            return invalid_ssntype_id;
        }
        BOOST_THROW_EXCEPTION(InternalError("failed to get ssntype"));
    }
    return invalid_ssntype_id;
}

/**
* Gets two character country code.
* @param country is name of the country or country code
* @param ctx contains reference to the database and logging interface
* @param ex_ptr is pointer to exception instance to set in case of failure,
* if nullptr, new exception instance is created, set and thrown in case of failure,
* if not nullptr, referred exception instance is only set
* @param ex_setter is exception member pointer used to set data into exception in case of failure
* @return two character country code or an empty string indicating "no country code found"
*/
template <class EXCEPTION, typename EXCEPTION_SETTER>
std::string get_country_code(
        const Optional<std::string>& country,
        OperationContext& ctx,
        EXCEPTION* ex_ptr,
        EXCEPTION_SETTER ex_setter)
{
    const std::string no_country_code;
    if (country.isset())
    {
        const Database::Result country_code_res = ctx.get_conn().exec_params(
                "SELECT id FROM enum_country "
                "WHERE id=$1::TEXT OR "
                      "country=$1::TEXT OR "
                      "country_cs=$1::TEXT",
                Database::query_param_list(country.get_value()));
        if (country_code_res.size() == 1)
        {
            return static_cast<std::string>(country_code_res[0][0]);
        }
        if (country_code_res.size() == 0)
        {
            if (ex_ptr == nullptr)
            {
                BOOST_THROW_EXCEPTION((EXCEPTION().*ex_setter)(country.get_value()));
            }
            (ex_ptr->*ex_setter)(country.get_value());
            return no_country_code;
        }
        BOOST_THROW_EXCEPTION(InternalError("failed to get country"));
    }
    return no_country_code;
}

}//namespace LibFred::Contact
}//namespace LibFred

#endif//CONTACT_ENUM_HH_54DADDB16CD34A4BAABBA444F473BE6B
