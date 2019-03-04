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
#include "libfred/registrar/exceptions.hh"
#include "libfred/registrar/update_registrar.hh"
#include "src/util/db/query_param.hh"
#include "src/util/util.hh"

#include <sstream>
#include <stdexcept>

namespace LibFred {
namespace Registrar {

namespace
{
constexpr const char * psql_type(const std::string&)
{
    return "::text";
}

constexpr const char * psql_type(const bool)
{
    return "::bool";
}

bool is_country_code_valid(LibFred::OperationContext& _ctx, const std::string& _country) {
    const Database::Result db_result = _ctx.get_conn().exec_params(
                "SELECT 1 FROM enum_country WHERE id = $1::text FOR SHARE ",
                Database::query_param_list(_country));
    return db_result.size() > 0;
}

} // namespace LibFred::Registrar::{anonymous}

UpdateRegistrar::UpdateRegistrar(const std::string& _handle)
        : handle_(_handle)
{
}

UpdateRegistrar& UpdateRegistrar::set_ico(const boost::optional<std::string>& _ico)
{
    ico_ = _ico;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_dic(const boost::optional<std::string>& _dic)
{
    dic_ = _dic;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_variable_symbol(const boost::optional<std::string>& _variable_symbol)
{
    variable_symbol_ = _variable_symbol;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_vat_payer(const boost::optional<bool>& _vat_payer)
{
    vat_payer_ = _vat_payer;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_name(const boost::optional<std::string>& _name)
{
    name_ = _name;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_organization(const boost::optional<std::string>& _organization)
{
    organization_ = _organization;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_street1(const boost::optional<std::string>& _street1)
{
    street1_ = _street1;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_street2(const boost::optional<std::string>& _street2)
{
    street2_ = _street2;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_street3(const boost::optional<std::string>& _street3)
{
    street3_ = _street3;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_city(const boost::optional<std::string>& _city)
{
    city_ = _city;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_state_or_province(
        const boost::optional<std::string>& _state_or_province)
{
    state_or_province_ = _state_or_province;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_postal_code(const boost::optional<std::string>& _postal_code)
{
    postal_code_ = _postal_code;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_country(const boost::optional<std::string>& _country)
{
    country_ = _country;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_telephone(const boost::optional<std::string>& _telephone)
{
    telephone_ = _telephone;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_fax(const boost::optional<std::string>& _fax)
{
    fax_ = _fax;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_email(const boost::optional<std::string>& _email)
{
    email_ = _email;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_url(const boost::optional<std::string>& _url)
{
    url_ = _url;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_system(const boost::optional<bool>& _system)
{
    system_ = _system;
    return *this;
}

UpdateRegistrar& UpdateRegistrar::set_payment_memo_regex(
        const boost::optional<std::string>& _payment_memo_regex)
{
    payment_memo_regex_ = _payment_memo_regex;
    return *this;
}

unsigned long long UpdateRegistrar::exec(OperationContext& _ctx) const
{
    const bool values_for_update_are_set = (ico_ != boost::none ||
            dic_ != boost::none ||
            variable_symbol_ != boost::none ||
            vat_payer_ != boost::none ||
            name_ != boost::none ||
            organization_ != boost::none ||
            street1_ != boost::none ||
            street2_ != boost::none ||
            street3_ != boost::none ||
            city_ != boost::none ||
            state_or_province_ != boost::none ||
            postal_code_ != boost::none ||
            country_ != boost::none ||
            telephone_ != boost::none ||
            fax_ != boost::none ||
            email_ != boost::none ||
            url_ != boost::none ||
            system_ != boost::none ||
            payment_memo_regex_ != boost::none);

    if (!values_for_update_are_set)
    {
        throw NoUpdateData();
    }

    Database::QueryParams params;
    std::ostringstream object_sql;
    Util::HeadSeparator set_separator(" SET ", ", ");

    object_sql << "UPDATE registrar";
    if (ico_ != boost::none && !ico_.value().empty())
    {
        params.push_back(ico_.value());
        object_sql << set_separator.get() << "ico = $" << params.size() << psql_type(ico_.value());
    }
    if (dic_ != boost::none && !dic_.value().empty())
    {
        params.push_back(dic_.value());
        object_sql << set_separator.get() << "dic = $" << params.size() << psql_type(dic_.value());
    }
    if (variable_symbol_ != boost::none && !variable_symbol_.value().empty())
    {
        params.push_back(variable_symbol_.value());
        object_sql << set_separator.get() << "varsymb = $" << params.size() << psql_type(variable_symbol_.value());
    }
    if (vat_payer_ != boost::none)
    {
        params.push_back(vat_payer_.value());
        object_sql << set_separator.get() << "vat = $" << params.size() << psql_type(vat_payer_.value());
    }
    if (name_ != boost::none && !name_.value().empty())
    {
        params.push_back(name_.value());
        object_sql << set_separator.get() << "name = $" << params.size() << psql_type(name_.value());
    }
    if (organization_ != boost::none && !organization_.value().empty())
    {
        params.push_back(organization_.value());
        object_sql << set_separator.get() << "organization = $" << params.size() << psql_type(organization_.value());
    }
    if (street1_ != boost::none && !street1_.value().empty())
    {
        params.push_back(street1_.value());
        object_sql << set_separator.get() << "street1 = $" << params.size() << psql_type(street1_.value());
    }
    if (street2_ != boost::none && !street2_.value().empty())
    {
        params.push_back(street2_.value());
        object_sql << set_separator.get() << "street2 = $" << params.size() << psql_type(street2_.value());
    }
    if (street3_ != boost::none && !street3_.value().empty())
    {
        params.push_back(street3_.value());
        object_sql << set_separator.get() << "street3 = $" << params.size() << psql_type(street3_.value());
    }
    if (city_ != boost::none && !city_.value().empty())
    {
        params.push_back(city_.value());
        object_sql << set_separator.get() << "city = $" << params.size() << psql_type(city_.value());
    }
    if (postal_code_ != boost::none && !postal_code_.value().empty())
    {
        params.push_back(postal_code_.value());
        object_sql << set_separator.get() << "postalcode = $" << params.size() << psql_type(postal_code_.value());
    }
    if (state_or_province_ != boost::none && !state_or_province_.value().empty())
    {
        params.push_back(state_or_province_.value());
        object_sql << set_separator.get() << "stateorprovince = $" << params.size() << psql_type(state_or_province_.value());
    }
    if (country_ != boost::none && !country_.value().empty())
    {
        if (!is_country_code_valid(_ctx, country_.value()))
        {
            throw UnknownCountryCode();
        }
        params.push_back(country_.value());
        object_sql << set_separator.get() << "country = $" << params.size() << psql_type(country_.value());
    }
    if (telephone_ != boost::none && !telephone_.value().empty())
    {
        params.push_back(telephone_.value());
        object_sql << set_separator.get() << "telephone = $" << params.size() << psql_type(telephone_.value());
    }
    if (fax_ != boost::none && !fax_.value().empty())
    {
        params.push_back(fax_.value());
        object_sql << set_separator.get() << "fax = $" << params.size() << psql_type(fax_.value());
    }
    if (email_ != boost::none && !email_.value().empty())
    {
        params.push_back(email_.value());
        object_sql << set_separator.get() << "email = $" << params.size() << psql_type(email_.value());
    }
    if (url_ != boost::none && !url_.value().empty())
    {
        params.push_back(url_.value());
        object_sql << set_separator.get() << "url = $" << params.size() << psql_type(url_.value());
    }
    if (system_ != boost::none)
    {
        params.push_back(system_.value());
        object_sql << set_separator.get() << "system = $" << params.size() << psql_type(system_.value());
    }
    if (payment_memo_regex_ != boost::none && !payment_memo_regex_.value().empty())
    {
        params.push_back(payment_memo_regex_.value());
        object_sql << set_separator.get() << "regex = $" << params.size() << psql_type(payment_memo_regex_.value());
    }

    params.push_back(handle_);
    object_sql << " WHERE registrar.handle = UPPER($" << params.size() << psql_type(handle_) << ") RETURNING id";

    try
    {
        const Database::Result update_result = _ctx.get_conn().exec_params(
                object_sql.str(),
                params);
        if (update_result.size() == 1)
        {
            const auto id = static_cast<unsigned long long>(update_result[0][0]);
            return id;
        }
        if (update_result.size() < 1)
        {
            throw NonExistentRegistrar();
        }
        throw std::runtime_error("Duplicity in database");
    }
    catch (const NonExistentRegistrar&)
    {
        throw;
    }
    catch (const std::exception&)
    {
        throw UpdateRegistrarException();
    }
}

} // namespace LibFred::Registrar
} // namespace LibFred
