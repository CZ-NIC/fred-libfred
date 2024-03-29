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
 *  registrar info data diff
 */

#ifndef INFO_REGISTRAR_DIFF_HH_C738A5AC10834248935E9252BDA08FD2
#define INFO_REGISTRAR_DIFF_HH_C738A5AC10834248935E9252BDA08FD2

#include "libfred/registrar/info_registrar_data.hh"

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <string>

namespace LibFred {

/**
 * Diff of registrar data.
 * Data of the registrar difference with the same members as registrar data but in optional pairs. Optional pair member is set in case of difference in compared registrar data.
 */
struct InfoRegistrarDiff : Util::Printable<InfoRegistrarDiff>
{
    /**
    * Constructor of the registrar data diff structure.
    */
    InfoRegistrarDiff();

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;

    /**
    * Check if some data is set into the instance
    * @return false if instance contains differing data and true if not
    */
    bool is_empty() const;

    template <class T> struct DiffMemeber { typedef Optional<std::pair<T, T> > Type;};

    DiffMemeber<std::string>::Type handle;/**< registrar handle */
    DiffMemeber<Nullable<std::string> >::Type name;/**< name of the registrar */
    DiffMemeber<Nullable<std::string> >::Type organization;/**< full trade name of organization */
    DiffMemeber<Nullable<std::string> >::Type street1;/**< part of address */
    DiffMemeber<Nullable<std::string> >::Type street2;/**< part of address */
    DiffMemeber<Nullable<std::string> >::Type street3;/**< part of address*/
    DiffMemeber<Nullable<std::string> >::Type city;/**< part of address - city */
    DiffMemeber<Nullable<std::string> >::Type stateorprovince;/**< part of address - region */
    DiffMemeber<Nullable<std::string> >::Type postalcode;/**< part of address - postal code */
    DiffMemeber<Nullable<std::string> >::Type country;/**< two character country code or country name */
    DiffMemeber<Nullable<std::string> >::Type telephone;/**<  telephone number */
    DiffMemeber<Nullable<std::string> >::Type fax;/**< fax number */
    DiffMemeber<Nullable<std::string> >::Type email;/**< e-mail address */
    DiffMemeber<Nullable<std::string> >::Type url;/**< web page of the registrar */
    DiffMemeber<Nullable<bool> >::Type system;/**< system registrar flag */
    DiffMemeber<Nullable<std::string> >::Type ico;/**< company registration number */
    DiffMemeber<Nullable<std::string> >::Type dic;/**< taxpayer identification number */
    DiffMemeber<Nullable<std::string> >::Type variable_symbol;/**< registrar payments coupling tag, have to match with payment variable symbol to couple payment with registrar*/
    DiffMemeber<Nullable<std::string> >::Type payment_memo_regex;/**< registrar payments coupling alternative to variable symbol, if payment_memo_regex is set, payment_memo have to match case insesitive with payment_memo_regex to couple payment with registrar*/
    DiffMemeber<bool>::Type vat_payer;/**< VAT payer flag */
    DiffMemeber<unsigned long long >::Type id; /**< registrar db id */
    DiffMemeber<bool>::Type is_internal;/**< internal registrar flag */
};

/**
 * Diff data of the registrar.
 * @param first
 * @param second
 * @return diff of given registrar
 */
InfoRegistrarDiff diff_registrar_data(const InfoRegistrarData& first, const InfoRegistrarData& second);

}//namespace LibFred

#endif//INFO_REGISTRAR_DIFF_HH_C738A5AC10834248935E9252BDA08FD2
