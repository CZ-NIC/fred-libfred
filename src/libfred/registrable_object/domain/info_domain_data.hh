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

#ifndef INFO_DOMAIN_DATA_HH_01A2CF8365C944A6B77108D7667AD395
#define INFO_DOMAIN_DATA_HH_01A2CF8365C944A6B77108D7667AD395

#include "libfred/object/object_id_handle_pair.hh"
#include "libfred/registrable_object/domain/enum_validation_extension.hh"
#include "libfred/registrable_object/uuid.hh"
#include "libfred/registrable_object/contact/contact_reference.hh"
#include "libfred/registrable_object/domain/domain_uuid.hh"
#include "libfred/registrable_object/keyset/keyset_reference.hh"
#include "libfred/registrable_object/nsset/nsset_reference.hh"

#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
 * Common data of domain.
 * Current or history state of the domain.
 */
struct InfoDomainData : Util::Printable<InfoDomainData>
{
    /**
    * Constructor of domain data structure.
    */
    InfoDomainData();

    /**
    * Equality of domain data structure operator.
    * @param rhs is right hand side of domain data comparison
    * @return true if equal, false if not
    */
    bool operator==(const InfoDomainData& rhs) const;

    /**
    * Inequality of the contact data structure operator.
    * @param rhs is right hand side of the contact data comparison
    * @return true if not equal, false if equal
    */
    bool operator!=(const InfoDomainData& rhs) const;

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;

    std::string roid;/**< registry object identifier of domain */
    std::string fqdn;/**< fully qualified domain name */
    RegistrableObject::Contact::ContactReference registrant;/**< registrant contact reference, owner of the domain*/
    Nullable<RegistrableObject::Nsset::NssetReference> nsset;/**< nsset reference or NULL if missing */
    Nullable<RegistrableObject::Keyset::KeysetReference> keyset;/**< keyset reference or NULL if missing */
    std::string sponsoring_registrar_handle;/**< handle of registrar administering domain */
    std::string create_registrar_handle;/**< handle of registrar which created domain */
    Nullable<std::string> update_registrar_handle;/**< handle of registrar which last time changed domain*/
    boost::posix_time::ptime creation_time;/**< time of domain creation in set local zone*/
    Nullable<boost::posix_time::ptime> update_time; /**< time of last update time in set local zone*/
    Nullable<boost::posix_time::ptime> transfer_time; /**< time of last transfer in set local zone*/
    boost::gregorian::date expiration_date; /**< domain expiration local date */
    std::string authinfopw;/**< unused */
    std::vector<RegistrableObject::Contact::ContactReference> admin_contacts;/**< list of administrative contacts (contacts references) */
    Nullable<ENUMValidationExtension> enum_domain_validation;/**< ENUM domain validation extension info */
    Nullable<boost::posix_time::ptime> delete_time;/**< domain delete time in set local zone*/
    unsigned long long historyid;/**< last historyid of domain history*/
    unsigned long long crhistoryid;/**< first historyid of domain history*/
    unsigned long long id;/**< id of the domain object*/
    RegistrableObject::Domain::DomainUuid uuid;/**< uuid of the object*/
    RegistrableObject::Domain::DomainHistoryUuid history_uuid;/**< history uuid of the domain object*/
    ObjectIdHandlePair zone;/**< zone id and fqdn of domain*/
};

}//namespace LibFred

#endif//INFO_DOMAIN_DATA_HH_01A2CF8365C944A6B77108D7667AD395
