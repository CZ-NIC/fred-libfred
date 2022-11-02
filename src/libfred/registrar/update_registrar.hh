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

#ifndef UPDATE_REGISTRAR_HH_0E1AC23F00484A6AA187CC5411670C8C
#define UPDATE_REGISTRAR_HH_0E1AC23F00484A6AA187CC5411670C8C

#include "libfred/opcontext.hh"

#include <boost/optional.hpp>

#include <string>
#include <vector>

namespace LibFred {
namespace Registrar {

class UpdateRegistrarById {

public:
    explicit UpdateRegistrarById(unsigned long long _id);

    UpdateRegistrarById& set_handle(boost::optional<std::string> _handle);

    UpdateRegistrarById& set_ico(boost::optional<std::string> _ico);

    UpdateRegistrarById& set_dic(boost::optional<std::string> _dic);

    UpdateRegistrarById& set_variable_symbol(boost::optional<std::string> _variable_symbol);

    UpdateRegistrarById& set_vat_payer(boost::optional<bool> _vat_payer);

    UpdateRegistrarById& set_name(boost::optional<std::string> _name);

    UpdateRegistrarById& set_organization(boost::optional<std::string> _organization);

    UpdateRegistrarById& set_street(boost::optional<std::vector<std::string>> _street);
    UpdateRegistrarById& set_street(std::string _street1);
    UpdateRegistrarById& set_street(std::string _street1, std::string _street2);
    UpdateRegistrarById& set_street(std::string _street1, std::string _street2, std::string _street3);

    UpdateRegistrarById& set_city(boost::optional<std::string> _city);

    UpdateRegistrarById& set_state_or_province(boost::optional<std::string> _state_or_province);

    UpdateRegistrarById& set_postal_code(boost::optional<std::string> _postal_code);

    UpdateRegistrarById& set_country(boost::optional<std::string> _country);

    UpdateRegistrarById& set_telephone(boost::optional<std::string> _telephone);

    UpdateRegistrarById& set_fax(boost::optional<std::string> _fax);

    UpdateRegistrarById& set_email(boost::optional<std::string> _email);

    UpdateRegistrarById& set_url(boost::optional<std::string> _url);

    UpdateRegistrarById& set_system(boost::optional<bool> _system);

    UpdateRegistrarById& set_payment_memo_regex(boost::optional<std::string> _payment_memo_regex);

    UpdateRegistrarById& set_internal(boost::optional<bool> value);

    void exec(const OperationContext& _ctx) const;
private:
    unsigned long long id_;
    boost::optional<std::string> handle_;
    boost::optional<std::string> ico_;
    boost::optional<std::string> dic_;
    boost::optional<std::string> variable_symbol_;
    boost::optional<bool> vat_payer_;
    boost::optional<std::string> name_;
    boost::optional<std::string> organization_;
    boost::optional<std::vector<std::string>> street_;
    boost::optional<std::string> city_;
    boost::optional<std::string> state_or_province_;
    boost::optional<std::string> postal_code_;
    boost::optional<std::string> country_;
    boost::optional<std::string> telephone_;
    boost::optional<std::string> fax_;
    boost::optional<std::string> email_;
    boost::optional<std::string> url_;
    boost::optional<bool> system_;
    boost::optional<std::string> payment_memo_regex_;
    boost::optional<bool> is_internal_;
};

} // namespace LibFred::Registrar
} // namespace LibFred

#endif
