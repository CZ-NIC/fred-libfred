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

#include "libfred/registrar/create_registrar.hh"

#include "libfred/registrable_object/contact/contact_enum.hh"
#include "util/util.hh"

#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <utility>

namespace LibFred {

namespace {

bool is_empty(const std::string& value)
{
    return value.empty() || std::all_of(begin(value), end(value), [](unsigned char c) { return std::isspace(c); });
}

template <typename Fnc, typename ...Args>
std::string nonempty(std::string value, Fnc make_exception, Args&& ...args)
{
    if (is_empty(value))
    {
        throw make_exception(std::forward<Args>(args)...);
    }
    return value;
}

auto make_missing_street_exception()
{
    CreateRegistrar::Exception e;
    e.set_missing_mandatory_attribute("street");
    return e;
}

auto make_missing_attribute_exception(const std::string& attribute_name)
{
    CreateRegistrar::Exception e;
    e.set_missing_mandatory_attribute(attribute_name);
    return e;
}

std::vector<std::string> correct_street(std::vector<std::string> street)
{
    static constexpr auto max_number_of_streets = 3u;
    if (max_number_of_streets < street.size())
    {
        CreateRegistrar::Exception e;
        e.set_too_many_streets(street.size());
        throw e;
    }
    if (street.empty())
    {
        throw make_missing_street_exception();
    }
    std::for_each(begin(street), end(street), [](auto&& item) { nonempty(item, make_missing_street_exception); });
    return street;
}

}//namespace LibFred::{anonymous}

CreateRegistrar::CreateRegistrar(
        std::string handle,
        std::string name,
        std::string organization,
        std::vector<std::string> street,
        std::string city,
        std::string postalcode,
        std::string telephone,
        std::string email,
        std::string url,
        std::string dic,
        bool system,
        bool is_internal)
    : handle_{nonempty(std::move(handle), make_missing_attribute_exception, "handle")},
      name_{nonempty(std::move(name), make_missing_attribute_exception, "name")},
      organization_{nonempty(std::move(organization), make_missing_attribute_exception, "organization")},
      street_{correct_street(std::move(street))},
      city_{nonempty(std::move(city), make_missing_attribute_exception, "city")},
      postalcode_{nonempty(std::move(postalcode), make_missing_attribute_exception, "postalcode")},
      telephone_{nonempty(std::move(telephone), make_missing_attribute_exception, "telephone")},
      email_{nonempty(std::move(email), make_missing_attribute_exception, "email")},
      url_{nonempty(std::move(url), make_missing_attribute_exception, "url")},
      system_{system},
      dic_{nonempty(std::move(dic), make_missing_attribute_exception, "dic")},
      is_internal_{is_internal}
{}

CreateRegistrar::CreateRegistrar(
        std::string handle,
        std::string name,
        std::string organization,
        std::vector<std::string> street,
        std::string city,
        const Optional<std::string>& stateorprovince,
        std::string postalcode,
        const Optional<std::string>& country,
        std::string telephone,
        const Optional<std::string>& fax,
        std::string email,
        std::string url,
        bool system,
        const Optional<std::string>& ico,
        std::string dic,
        const Optional<std::string>& variable_symbol,
        const Optional<std::string>& payment_memo_regex,
        const Optional<bool>& vat_payer,
        bool is_internal)
    : handle_{nonempty(std::move(handle), make_missing_attribute_exception, "handle")},
      name_{nonempty(std::move(name), make_missing_attribute_exception, "name")},
      organization_{nonempty(std::move(organization), make_missing_attribute_exception, "organization")},
      street_{correct_street(std::move(street))},
      city_{nonempty(std::move(city), make_missing_attribute_exception, "city")},
      stateorprovince_{stateorprovince},
      postalcode_{nonempty(std::move(postalcode), make_missing_attribute_exception, "postalcode")},
      country_{country},
      telephone_{nonempty(std::move(telephone), make_missing_attribute_exception, "telephone")},
      fax_{fax},
      email_{nonempty(std::move(email), make_missing_attribute_exception, "email")},
      url_{nonempty(std::move(url), make_missing_attribute_exception, "url")},
      system_{system},
      ico_{ico},
      dic_{nonempty(std::move(dic), make_missing_attribute_exception, "dic")},
      variable_symbol_{variable_symbol},
      payment_memo_regex_{payment_memo_regex},
      vat_payer_{vat_payer},
      is_internal_{is_internal}
{}

CreateRegistrar& CreateRegistrar::set_name(std::string name)
{
    name_ = nonempty(std::move(name), make_missing_attribute_exception, "name");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_organization(std::string organization)
{
    organization_ = nonempty(std::move(organization), make_missing_attribute_exception, "organization");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_street(std::vector<std::string> street)
{
    street_ = correct_street(std::move(street));
    return *this;
}

CreateRegistrar& CreateRegistrar::set_street(std::string street1)
{
    street_ = {nonempty(std::move(street1), make_missing_street_exception)};
    return *this;
}

CreateRegistrar& CreateRegistrar::set_street(std::string street1, std::string street2)
{
    street_ =
            {
                nonempty(std::move(street1), make_missing_street_exception),
                nonempty(std::move(street2), make_missing_street_exception)
            };
    return *this;
}

CreateRegistrar& CreateRegistrar::set_street(std::string street1, std::string street2, std::string street3)
{
    street_ =
            {
                nonempty(std::move(street1), make_missing_street_exception),
                nonempty(std::move(street2), make_missing_street_exception),
                nonempty(std::move(street3), make_missing_street_exception)
            };
    return *this;
}

CreateRegistrar& CreateRegistrar::set_city(std::string city)
{
    city_ = nonempty(std::move(city), make_missing_attribute_exception, "city");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_stateorprovince(const std::string& stateorprovince)
{
    stateorprovince_ = stateorprovince;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_postalcode(std::string postalcode)
{
    postalcode_ = nonempty(std::move(postalcode), make_missing_attribute_exception, "postalcode");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_country(const std::string& country)
{
    country_ = country;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_telephone(std::string telephone)
{
    telephone_ = nonempty(std::move(telephone), make_missing_attribute_exception, "telephone");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_fax(const std::string& fax)
{
    fax_ = fax;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_email(std::string email)
{
    email_ = nonempty(std::move(email), make_missing_attribute_exception, "email");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_url(std::string url)
{
    url_ = nonempty(std::move(url), make_missing_attribute_exception, "url");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_system(bool system)
{
    system_ = system;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_ico(const std::string& ico)
{
    ico_ = ico;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_dic(std::string dic)
{
    dic_ = nonempty(std::move(dic), make_missing_attribute_exception, "dic");
    return *this;
}

CreateRegistrar& CreateRegistrar::set_variable_symbol(const std::string& variable_symbol)
{
    variable_symbol_ = variable_symbol;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_payment_memo_regex(const std::string& payment_memo_regex)
{
    payment_memo_regex_ = payment_memo_regex;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_vat_payer(bool vat_payer)
{
    vat_payer_ = vat_payer;
    return *this;
}

CreateRegistrar& CreateRegistrar::set_internal(bool value)
{
    is_internal_ = value;
    return *this;
}

unsigned long long CreateRegistrar::exec(const OperationContext& ctx)
{
    try
    {
        Database::QueryParams params;
        std::ostringstream col_sql;
        std::ostringstream val_sql;

        col_sql << "INSERT INTO registrar (handle, name, organization, city, postalcode, telephone, email, url, dic, system, is_internal";
        params = {handle_, name_, organization_, city_, postalcode_, telephone_, email_, url_, dic_, system_, is_internal_};
        val_sql << "UPPER($1::TEXT), $2::TEXT, $3::TEXT, $4::TEXT, $5::TEXT, $6::TEXT, $7::TEXT, $8::TEXT, $9::TEXT, $10::BOOL, $11::BOOL";

        if (0 < street_.size())
        {
            params.push_back(street_[0]);
            col_sql << ", street1";
            val_sql << ", $" << params.size() << "::TEXT";
            if (1 < street_.size())
            {
                params.push_back(street_[1]);
                col_sql << ", street2";
                val_sql << ", $" << params.size() << "::TEXT";
                if (2 < street_.size())
                {
                    params.push_back(street_[2]);
                    col_sql << ", street3";
                    val_sql << ", $" << params.size() << "::TEXT";
                }
            }
        }

        if (stateorprovince_.isset() && !stateorprovince_.get_value().empty())
        {
            params.push_back(stateorprovince_.get_value());
            col_sql << ", stateorprovince";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (country_.isset() && !country_.get_value().empty())
        {
            params.push_back(LibFred::Contact::get_country_code(country_, ctx, static_cast<Exception*>(nullptr),
                            &Exception::set_unknown_country)); //throw if country unknown
            col_sql << ", country";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (fax_.isset() && !fax_.get_value().empty())
        {
            params.push_back(fax_.get_value());
            col_sql << ", fax";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (ico_.isset() && !ico_.get_value().empty())
        {
            params.push_back(ico_.get_value());
            col_sql << ", ico";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (variable_symbol_.isset() && !variable_symbol_.get_value().empty())
        {
            params.push_back(variable_symbol_.get_value());
            col_sql << ", varsymb";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (payment_memo_regex_.isset() && !payment_memo_regex_.get_value().empty())
        {
            params.push_back(payment_memo_regex_.get_value());
            col_sql << ", regex";
            val_sql << ", $" << params.size() << "::TEXT";
        }

        if (vat_payer_.isset())
        {
            params.push_back(vat_payer_.get_value());
            col_sql << ", vat";
            val_sql << ", $" << params.size() << "::BOOL";
        }

        //insert into registrar
        try
        {
            const Database::Result dbres = ctx.get_conn().exec_params(col_sql.str() + " VALUES(" + val_sql.str() + ") RETURNING id", params);

            if (dbres.size() != 1)
            {
                BOOST_THROW_EXCEPTION(LibFred::InternalError("registrar creation failed"));
            }
            const auto id = static_cast<unsigned long long>(dbres[0][0]);
            return id;
        }
        catch (const std::exception& e)
        {
            std::string what_string(e.what());
            if (what_string.find("registrar_handle_key") != std::string::npos)
            {
                BOOST_THROW_EXCEPTION(Exception().set_invalid_registrar_handle(handle_));
            }
            if (what_string.find("registrar_varsymb_key") != std::string::npos)
            {
                BOOST_THROW_EXCEPTION(Exception().set_invalid_registrar_varsymb(variable_symbol_.get_value()));
            }
            throw;
        }
    }
    catch (ExceptionStack& e)
    {
        e.add_exception_stack_info(this->to_string());
        throw;
    }
}

std::string CreateRegistrar::to_string() const
{
    return Util::format_operation_state(
            "CreateRegistrar",
            Util::vector_of<std::pair<std::string,std::string>>
            (std::make_pair("handle", handle_))
            (std::make_pair("name", name_))
            (std::make_pair("organization", organization_))
            (std::make_pair("street1", street_[0]))
            (std::make_pair("street2", street_[1]))
            (std::make_pair("street3", street_[2]))
            (std::make_pair("city", city_))
            (std::make_pair("stateorprovince", stateorprovince_.print_quoted()))
            (std::make_pair("postalcode", postalcode_))
            (std::make_pair("country", country_.print_quoted()))
            (std::make_pair("telephone", telephone_))
            (std::make_pair("fax", fax_.print_quoted()))
            (std::make_pair("email", email_))
            (std::make_pair("url",url_))
            (std::make_pair("system", std::to_string(system_)))
            (std::make_pair("ico", ico_.print_quoted()))
            (std::make_pair("dic", dic_))
            (std::make_pair("variable_symbol", variable_symbol_.print_quoted()))
            (std::make_pair("payment_memo_regex", payment_memo_regex_.print_quoted()))
            (std::make_pair("vat_payer", vat_payer_.print_quoted()))
            (std::make_pair("is_internal", std::to_string(is_internal_))));
}

const std::string& CreateRegistrar::get_handle() const
{
    return handle_;
}

}//namespace LibFred
