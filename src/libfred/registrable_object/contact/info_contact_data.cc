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

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/util.hh"
#include "libfred/registrable_object/contact/info_contact_data.hh"
#include "libfred/registrable_object/contact/info_contact_diff.hh"

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>

namespace LibFred {

std::string ContactAddressType::to_string(Value _value)
{
    switch (_value) {
        case MAILING:
            return "MAILING";
        case BILLING:
            return "BILLING";
        case SHIPPING:
            return "SHIPPING";
        case SHIPPING_2:
            return "SHIPPING_2";
        case SHIPPING_3:
            return "SHIPPING_3";
    }
    std::ostringstream msg;
    msg << "invalid value " << static_cast< int >(_value) << " of ContactAddressType";
    throw ConversionError(msg.str());
}

ContactAddressType::Value ContactAddressType::from_string(const std::string &_src)
{
    if (_src == "MAILING") {
        return MAILING;
    }
    if (_src == "BILLING") {
        return BILLING;
    }
    if (_src == "SHIPPING") {
        return SHIPPING;
    }
    if (_src == "SHIPPING_2") {
        return SHIPPING_2;
    }
    if (_src == "SHIPPING_3") {
        return SHIPPING_3;
    }
    std::ostringstream msg;
    msg << "\"" << _src << "\" unable convert to ContactAddressType";
    throw ConversionError(msg.str());
}

std::string ContactAddress::to_string()const
{
    return Util::format_data_structure("ContactAddress",
    Util::vector_of< std::pair< std::string, std::string > >
    (std::make_pair("place", static_cast< const Contact::PlaceAddress& >(*this).to_string()))
    (std::make_pair("company_name", company_name.print_quoted()))
    );
}

bool ContactAddress::operator==(const ContactAddress &_b)const
{
    return static_cast< const Contact::PlaceAddress& >(*this) == static_cast< const Contact::PlaceAddress& >(_b) &&
           this->company_name == _b.company_name;
}

InfoContactData::InfoContactData()
    : crhistoryid(0),
      historyid(0),
      disclosename(false),
      discloseorganization(false),
      discloseaddress(true),
      disclosetelephone(false),
      disclosefax(false),
      discloseemail(false),
      disclosevat(false),
      discloseident(false),
      disclosenotifyemail(false),
      id(0),
      uuid(),
      history_uuid(),
      addresses(),
      warning_letter()
{}

InfoContactData::Address InfoContactData::get_permanent_address()const
{
    if (place.isnull())
    {
        throw AddressDoesntExist("no address present");
    }
    Address address(
        Optional<std::string>(),
        Optional<std::string>(),
        ContactAddress(
            Optional<std::string>(),
            place.get_value()));
    if (!name.isnull())
    {
        address.name = name.get_value();
    }
    if (!organization.isnull())
    {
        address.organization = organization.get_value();
    }
    return address;
}

template < ContactAddressType::Value purpose >
InfoContactData::Address InfoContactData::get_address()const
{
    ContactAddressList::const_iterator ptr_contact_address = addresses.find(purpose);
    if (ptr_contact_address != addresses.end()) {
        Address address(
            Optional<std::string>(),
            Optional<std::string>(),
            ptr_contact_address->second
        );
        if (!name.isnull()) {
            address.name = name.get_value();
        }
        if (!organization.isnull()) {
            address.organization = organization.get_value();
        }
        return address;
    }
    return this->get_permanent_address();
}

template
InfoContactData::Address InfoContactData::get_address< ContactAddressType::MAILING >()const;

template
InfoContactData::Address InfoContactData::get_address< ContactAddressType::BILLING >()const;

template
InfoContactData::Address InfoContactData::get_address< ContactAddressType::SHIPPING >()const;

template
InfoContactData::Address InfoContactData::get_address< ContactAddressType::SHIPPING_2 >()const;

template
InfoContactData::Address InfoContactData::get_address< ContactAddressType::SHIPPING_3 >()const;

bool InfoContactData::operator==(const InfoContactData& rhs) const
{
    return diff_contact_data(*this, rhs).is_empty();
}

bool InfoContactData::operator!=(const InfoContactData& rhs) const
{
    return !this->operator ==(rhs);
}

namespace {

template <typename K, typename V>
std::string format_map(const std::map<K, V> &in)
{
    std::ostringstream out;

    for (const auto& itr : in)
    {
        if (!out.str().empty())
        {
            out << " ";
        }
        out << "\"" << itr.first << "\":\"" << itr.second << "\"";// "key":"value"
    }
    return out.str();
}

}//namespace LibFred::{anonymous}

std::string InfoContactData::to_string() const
{
    return Util::format_data_structure(
            "InfoContactData",
            {
                std::make_pair("crhistoryid", std::to_string(crhistoryid)),
                std::make_pair("historyid", std::to_string(historyid)),
                std::make_pair("id", std::to_string(id)),
                std::make_pair("uuid", strong_to_string(uuid)),
                std::make_pair("history_uuid", strong_to_string(history_uuid)),
                std::make_pair("delete_time", delete_time.print_quoted()),
                std::make_pair("handle", handle),
                std::make_pair("roid", roid),
                std::make_pair("sponsoring_registrar_handle", sponsoring_registrar_handle),
                std::make_pair("create_registrar_handle", create_registrar_handle),
                std::make_pair("update_registrar_handle", update_registrar_handle.print_quoted()),
                std::make_pair("creation_time", boost::lexical_cast<std::string>(creation_time)),
                std::make_pair("update_time", update_time.print_quoted()),
                std::make_pair("transfer_time", transfer_time.print_quoted()),
                std::make_pair("name", name.print_quoted()),
                std::make_pair("organization", organization.print_quoted()),
                std::make_pair("place", place.print_quoted()),
                std::make_pair("telephone", telephone.print_quoted()),
                std::make_pair("fax", fax.print_quoted()),
                std::make_pair("email", email.print_quoted()),
                std::make_pair("notifyemail_", notifyemail.print_quoted()),
                std::make_pair("vat", vat.print_quoted()),
                std::make_pair("ssntype", ssntype.print_quoted()),
                std::make_pair("ssn", ssn.print_quoted()),
                std::make_pair("addresses", format_map(addresses)),
                std::make_pair("disclosename", disclosename ? "true" : "false"),
                std::make_pair("discloseorganization", discloseorganization ? "true" : "false"),
                std::make_pair("discloseaddress", discloseaddress ? "true" : "false"),
                std::make_pair("disclosetelephone", disclosetelephone ? "true" : "false"),
                std::make_pair("disclosefax", disclosefax ? "true" : "false"),
                std::make_pair("discloseemail", discloseemail ? "true" : "false"),
                std::make_pair("disclosevat", disclosevat ? "true" : "false"),
                std::make_pair("discloseident", discloseident ? "true" : "false"),
                std::make_pair("disclosenotifyemail", disclosenotifyemail ? "true" : "false"),
                std::make_pair("warning_letter", warning_letter.print_quoted())
            });
}

std::ostream& operator<<(std::ostream &os, const ContactAddressList &v)
{
    return os << format_map(v);
}

std::ostream& operator<<(std::ostream &_os, const ContactAddress &_v)
{
    return _os << _v.to_string();
}

std::ostream& operator<<(std::ostream &_os, const ContactAddressType &_v)
{
    return _os << _v.to_string();
}

}//namespace LibFred
