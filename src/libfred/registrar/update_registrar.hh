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
#ifndef UPDATE_REGISTRAR_HH_0E1AC23F00484A6AA187CC5411670C8C
#define UPDATE_REGISTRAR_HH_0E1AC23F00484A6AA187CC5411670C8C

#include "libfred/opcontext.hh"

#include <boost/optional.hpp>
#include <string>

namespace LibFred {
namespace Registrar {

class UpdateRegistrarById {

public:
    explicit UpdateRegistrarById(unsigned long long _id);

    UpdateRegistrarById& set_handle(const boost::optional<std::string>& _handle);

    UpdateRegistrarById& set_ico(const boost::optional<std::string>& _ico);

    UpdateRegistrarById& set_dic(const boost::optional<std::string>& _dic);

    UpdateRegistrarById& set_variable_symbol(const boost::optional<std::string>& _variable_symbol);

    UpdateRegistrarById& set_vat_payer(const boost::optional<bool>& _vat_payer);

    UpdateRegistrarById& set_name(const boost::optional<std::string>& _name);

    UpdateRegistrarById& set_organization(const boost::optional<std::string>& _organization);

    UpdateRegistrarById& set_street1(const boost::optional<std::string>& _street1);

    UpdateRegistrarById& set_street2(const boost::optional<std::string>& _street2);

    UpdateRegistrarById& set_street3(const boost::optional<std::string>& _street3);

    UpdateRegistrarById& set_city(const boost::optional<std::string>& _city);

    UpdateRegistrarById& set_state_or_province(const boost::optional<std::string>& _state_or_province);

    UpdateRegistrarById& set_postal_code(const boost::optional<std::string>& _postal_code);

    UpdateRegistrarById& set_country(const boost::optional<std::string>& _country);

    UpdateRegistrarById& set_telephone(const boost::optional<std::string>& _telephone);

    UpdateRegistrarById& set_fax(const boost::optional<std::string>& _fax);

    UpdateRegistrarById& set_email(const boost::optional<std::string>& _email);

    UpdateRegistrarById& set_url(const boost::optional<std::string>& _url);

    UpdateRegistrarById& set_system(const boost::optional<bool>& _system);

    UpdateRegistrarById& set_payment_memo_regex(const boost::optional<std::string>& _payment_memo_regex);

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
    boost::optional<std::string> street1_;
    boost::optional<std::string> street2_;
    boost::optional<std::string> street3_;
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
};

} // namespace LibFred::Registrar
} // namespace LibFred

#endif
