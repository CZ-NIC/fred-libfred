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

#ifndef INFO_DOMAIN_DIFF_HH_68B5D7A7671D46E7B7BCD2A93DEB43EF
#define INFO_DOMAIN_DIFF_HH_68B5D7A7671D46E7B7BCD2A93DEB43EF

#include "libfred/registrable_object/domain/info_domain_data.hh"

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <string>
#include <set>
#include <utility>

namespace LibFred {

/**
 * Diff of domain data.
 * Data of the domain difference with the same members as domain data but in optional pairs. Optional pair member is set in case of difference in compared domain data.
 */
struct InfoDomainDiff : Util::Printable<InfoDomainDiff>
{
    /**
     * Constructor of the domain data diff structure.
     */
    InfoDomainDiff();

    /**
     * Get names of set fields.
     * @return string names of fields that actually changed
     */
    std::set<std::string> changed_fields() const;

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

    template <typename T>
    using DiffMember = Optional<std::pair<T, T>>;

    DiffMember<unsigned long long> crhistoryid;/**< first historyid of domain history*/
    DiffMember<unsigned long long> historyid;/**< last historyid of domain history*/
    DiffMember<Nullable<boost::posix_time::ptime>> delete_time; /**< domain delete time in set local zone*/
    DiffMember<std::string> fqdn;/**< fully qualified domain name */
    DiffMember<std::string> roid;/**< registry object identifier of the domain */
    DiffMember<std::string> sponsoring_registrar_handle;/**< registrar administering the domain */
    DiffMember<std::string> create_registrar_handle;/**< registrar that created the domain */
    DiffMember<Nullable<std::string>> update_registrar_handle;/**< registrar which last time changed the domain */
    DiffMember<boost::posix_time::ptime> creation_time;/**< creation time of the domain in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime>> update_time; /**< last update time of the domain in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime>> transfer_time; /**<last transfer time in set local zone*/

    DiffMember<RegistrableObject::Contact::ContactReference> registrant; /**< registrant contact reference, owner of domain*/
    DiffMember<Nullable<RegistrableObject::Nsset::NssetReference>> nsset;/**< nsset id and handle or NULL if missing */
    DiffMember<Nullable<RegistrableObject::Keyset::KeysetReference>> keyset;/**< keyset id and handle or NULL if missing */
    DiffMember<boost::gregorian::date> expiration_date;/**< domain expiration local date */
    DiffMember<std::vector<RegistrableObject::Contact::ContactReference>> admin_contacts;/**< list of administrating contact handles */
    DiffMember<Nullable<ENUMValidationExtension>> enum_domain_validation;/**< ENUM domain validation extension info */
    DiffMember<LibFred::ObjectIdHandlePair> zone;/**< zone id and fqdn */

    DiffMember<unsigned long long> id;/**< id of the domain object*/
    DiffMember<RegistrableObject::Domain::DomainUuid> uuid;/**< uuid of the object*/
    DiffMember<RegistrableObject::Domain::DomainHistoryUuid> history_uuid;/**< history uuid of the domain object*/
};

/**
 * Diff data of the domain.
 * @param first
 * @param second
 * @return diff of given domain
 */
InfoDomainDiff diff_domain_data(const InfoDomainData& first, const InfoDomainData& second);

}//namespace LibFred

#endif//INFO_DOMAIN_DIFF_HH_68B5D7A7671D46E7B7BCD2A93DEB43EF
