/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

namespace LibFred {
namespace Registrar {

namespace {

constexpr const char * psql_type(const std::string&)
{
    return "::text";
}

constexpr const char * psql_type(bool)
{
    return "::bool";
}

constexpr const char * psql_type(unsigned long long)
{
    return "::bigint";
}

class InvalidAttribute : public UpdateRegistrarException
{
public:
    explicit InvalidAttribute(const char* message)
        : message_{message}
    { }
private:
    const char* what() const noexcept override { return message_; }
    const char* message_;
};

bool is_empty(const std::string& value)
{
    return value.empty() ||
           std::all_of(begin(value), end(value), [](unsigned char c) { return std::isspace(c); });
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

template <typename Fnc, typename ...Args>
boost::optional<std::string> nonempty(boost::optional<std::string> value, Fnc make_exception, Args&& ...args)
{
    if ((value != boost::none) && is_empty(*value))
    {
        throw make_exception(std::forward<Args>(args)...);
    }
    return value;
}

auto make_invalid_street_exception()
{
    return InvalidAttribute{"street can not be empty"};
}

auto make_invalid_attribute_exception(const char* message)
{
    return InvalidAttribute{message};
}

void check_street(const std::vector<std::string>& street)
{
    static constexpr auto max_number_of_streets = 3u;
    if (max_number_of_streets < street.size())
    {
        struct TooManyStreets : UpdateRegistrarException
        {
            const char* what() const noexcept override { return "too many street items"; }
        };
        throw TooManyStreets{};
    }
    if (street.empty())
    {
        throw make_invalid_street_exception();
    }
    std::for_each(begin(street), end(street), [](auto&& item)
    {
        nonempty(item, make_invalid_street_exception);
    });
}

boost::optional<std::vector<std::string>> correct_street(boost::optional<std::vector<std::string>> street)
{
    if (street != boost::none)
    {
        check_street(*street);
    }
    return street;
}

bool does_optional_attribute_present(const std::string& value)
{
    return !is_empty(value);
}

class TheSetPartOfUpdate
{
public:
    explicit TheSetPartOfUpdate(Database::QueryParams& params)
        : params_{params}
    { }
    template <typename T, typename Fnc>
    TheSetPartOfUpdate& append(const T& value, Fnc fnc)
    {
        if (!sql_.empty())
        {
            sql_.append(", ");
        }
        params_.push_back(value);
        sql_.append(fnc("$" + std::to_string(params_.size()) + psql_type(value)));
        return *this;
    }
    template <typename Fnc>
    TheSetPartOfUpdate& append_optional(const std::string& value, Fnc fnc)
    {
        if (!sql_.empty())
        {
            sql_.append(", ");
        }
        if (does_optional_attribute_present(value))
        {
            params_.push_back(value);
            sql_.append(fnc("$" + std::to_string(params_.size()) + psql_type(value)));
        }
        else
        {
            sql_.append(fnc("NULL::TEXT"));
        }
        return *this;
    }
    const std::string& get() const noexcept
    {
        return sql_;
    }
private:
    Database::QueryParams& params_;
    std::string sql_;
};

bool is_country_code_valid(const LibFred::OperationContext& _ctx, const std::string& _country)
{
    const auto db_result = _ctx.get_conn().exec_params(
            "SELECT FROM enum_country WHERE id = $1::TEXT FOR SHARE",
            Database::query_param_list(_country));
    return 0 < db_result.size();
}

} // namespace LibFred::Registrar::{anonymous}

UpdateRegistrarById::UpdateRegistrarById(const unsigned long long _id)
    : id_(_id)
{ }

UpdateRegistrarById& UpdateRegistrarById::set_handle(boost::optional<std::string> _handle)
{
    handle_ = nonempty(std::move(_handle), make_invalid_attribute_exception, "handle can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_ico(boost::optional<std::string> _ico)
{
    ico_ = std::move(_ico);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_dic(boost::optional<std::string> _dic)
{
    dic_ = nonempty(std::move(_dic), make_invalid_attribute_exception, "dic can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_variable_symbol(boost::optional<std::string> _variable_symbol)
{
    variable_symbol_ = std::move(_variable_symbol);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_vat_payer(boost::optional<bool> _vat_payer)
{
    vat_payer_ = _vat_payer;
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_name(boost::optional<std::string> _name)
{
    name_ = nonempty(std::move(_name), make_invalid_attribute_exception, "name can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_organization(boost::optional<std::string> _organization)
{
    organization_ = nonempty(std::move(_organization), make_invalid_attribute_exception, "organization can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_street(boost::optional<std::vector<std::string>> _street)
{
    street_ = correct_street(std::move(_street));
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_street(std::string _street1)
{
    street_ = std::vector<std::string>{nonempty(std::move(_street1), make_invalid_street_exception)};
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_street(std::string _street1, std::string _street2)
{
    street_ = std::vector<std::string>{
            nonempty(std::move(_street1), make_invalid_street_exception),
            nonempty(std::move(_street2), make_invalid_street_exception)
    };
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_street(std::string _street1, std::string _street2, std::string _street3)
{
    street_ = std::vector<std::string>{
            nonempty(std::move(_street1), make_invalid_street_exception),
            nonempty(std::move(_street2), make_invalid_street_exception),
            nonempty(std::move(_street3), make_invalid_street_exception)
    };
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_city(boost::optional<std::string> _city)
{
    city_ = nonempty(std::move(_city), make_invalid_attribute_exception, "city can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_state_or_province(
        boost::optional<std::string> _state_or_province)
{
    state_or_province_ = std::move(_state_or_province);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_postal_code(boost::optional<std::string> _postal_code)
{
    postal_code_ = nonempty(std::move(_postal_code), make_invalid_attribute_exception, "postal code can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_country(boost::optional<std::string> _country)
{
    country_ = std::move(_country);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_telephone(boost::optional<std::string> _telephone)
{
    telephone_ = nonempty(std::move(_telephone), make_invalid_attribute_exception, "telephone can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_fax(boost::optional<std::string> _fax)
{
    fax_ = std::move(_fax);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_email(boost::optional<std::string> _email)
{
    email_ = nonempty(std::move(_email), make_invalid_attribute_exception, "email can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_url(boost::optional<std::string> _url)
{
    url_ = nonempty(std::move(_url), make_invalid_attribute_exception, "url can not be empty");
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_system(boost::optional<bool> _system)
{
    system_ = _system;
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_payment_memo_regex(
        boost::optional<std::string> _payment_memo_regex)
{
    payment_memo_regex_ = std::move(_payment_memo_regex);
    return *this;
}

UpdateRegistrarById& UpdateRegistrarById::set_internal(boost::optional<bool> value)
{
    is_internal_ = value;
    return *this;
}

void UpdateRegistrarById::exec(const OperationContext& _ctx) const
{
    Database::QueryParams params;
    TheSetPartOfUpdate the_set_part{params};

    if (handle_ != boost::none)
    {
        the_set_part.append(*handle_, [](const std::string& param)
        {
            return "handle = UPPER(" + param + ")";
        });
    }
    if (ico_ != boost::none)
    {
        the_set_part.append_optional(*ico_, [](const std::string& param)
        {
            return "ico = " + param;
        });
    }
    if (dic_ != boost::none)
    {
        the_set_part.append(*dic_, [](const std::string& param)
        {
            return "dic = " + param;
        });
    }
    if (variable_symbol_ != boost::none)
    {
        the_set_part.append_optional(*variable_symbol_, [](const std::string& param)
        {
            return "varsymb = " + param;
        });
    }
    if (vat_payer_ != boost::none)
    {
        the_set_part.append(*vat_payer_, [](const std::string& param)
        {
            return "vat = " + param;
        });
    }
    if (name_ != boost::none)
    {
        the_set_part.append(*name_, [](const std::string& param)
        {
            return "name = " + param;
        });
    }
    if (organization_ != boost::none)
    {
        the_set_part.append(*organization_, [](const std::string& param)
        {
            return "organization = " + param;
        });
    }
    if (street_ != boost::none)
    {
        the_set_part.append((*street_)[0], [](const std::string& param)
        {
            return "street1 = " + param;
        });
        the_set_part.append_optional(1 < street_->size() ? (*street_)[1] : "",
                [](const std::string& param)
                {
                    return "street2 = " + param;
                });
        the_set_part.append_optional(2 < street_->size() ? (*street_)[2] : "",
                [](const std::string& param)
                {
                    return "street3 = " + param;
                });
    }
    if (city_ != boost::none)
    {
        the_set_part.append(*city_, [](const std::string& param)
        {
            return "city = " + param;
        });
    }
    if (postal_code_ != boost::none)
    {
        the_set_part.append(*postal_code_, [](const std::string& param)
        {
            return "postalcode = " + param;
        });
    }
    if (state_or_province_ != boost::none)
    {
        the_set_part.append_optional(*state_or_province_, [](const std::string& param)
        {
            return "stateorprovince = " + param;
        });
    }
    if (country_ != boost::none)
    {
        if (!is_country_code_valid(_ctx, *country_) &&
            !is_empty(*country_))
        {
            throw UnknownCountryCode();
        }
        the_set_part.append_optional(*country_, [](const std::string& param)
        {
            return "country = " + param;
        });
    }
    if (telephone_ != boost::none)
    {
        the_set_part.append(*telephone_, [](const std::string& param)
        {
            return "telephone = " + param;
        });
    }
    if (fax_ != boost::none)
    {
        the_set_part.append_optional(*fax_, [](const std::string& param)
        {
            return "fax = " + param;
        });
    }
    if (email_ != boost::none)
    {
        the_set_part.append(*email_, [](const std::string& param)
        {
            return "email = " + param;
        });
    }
    if (url_ != boost::none)
    {
        the_set_part.append(*url_, [](const std::string& param)
        {
            return "url = " + param;
        });
    }
    if (system_ != boost::none)
    {
        the_set_part.append(*system_, [](const std::string& param)
        {
            return "system = " + param;
        });
    }
    if (payment_memo_regex_)
    {
        the_set_part.append_optional(*payment_memo_regex_, [](const std::string& param)
        {
            return "regex = " + param;
        });
    }
    if (is_internal_)
    {
        the_set_part.append(*is_internal_, [](const std::string& param)
        {
            return "is_internal = " + param;
        });
    }

    if (the_set_part.get().empty())
    {
        throw NoUpdateData();
    }
    params.push_back(id_);
    const auto sql = "UPDATE registrar "
                        "SET " + the_set_part.get() + " "
                      "WHERE id = $" + std::to_string(params.size()) + psql_type(id_) + " "
                  "RETURNING 0";

    try
    {
        const auto update_result = _ctx.get_conn().exec_params(sql, params);
        if (1 < update_result.size())
        {
            struct BadDatabase : UpdateRegistrarException
            {
                const char* what() const noexcept override { return "Duplicity in database"; }
            };
            throw BadDatabase{};
        }
        if (update_result.size() < 1)
        {
            throw NonExistentRegistrar();
        }
    }
    catch (const NonExistentRegistrar&)
    {
        throw;
    }
    catch (const UpdateRegistrarException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        const std::string what_string(e.what());
        // there is no better way to detect an SQL error caused by a uniqueness violation
        if (what_string.find("registrar_handle_key") != std::string::npos)
        {
            throw RegistrarHandleAlreadyExists();
        }
        // there is no better way to detect an SQL error caused by a uniqueness violation
        if (what_string.find("registrar_varsymb_key") != std::string::npos)
        {
            throw VariableSymbolAlreadyExists();
        }
        throw UpdateRegistrarException();
    }
}

} // namespace LibFred::Registrar
} // namespace LibFred
