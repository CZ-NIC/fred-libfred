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

#ifndef INFO_CONTACT_DIFF_HH_741D8D1B6DF14EE1A97088C2CEE33E01
#define INFO_CONTACT_DIFF_HH_741D8D1B6DF14EE1A97088C2CEE33E01

#include "libfred/registrable_object/contact/info_contact_data.hh"

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <set>
#include <string>
#include <utility>

namespace LibFred {

/**
 * Diff of contact data.
 * Data of the contact difference with the same members as contact data but in optional pairs. Optional pair member is set in case of difference in compared contact data.
 */
struct InfoContactDiff : Util::Printable<InfoContactDiff>
{
    /**
    * Constructor of the contact data diff structure.
    */
    InfoContactDiff();

    /**
    * Get names of set fields.
    * @return string names of fields that actually changed
    */
    std::set<std::string> changed_fields()const;

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;

    /**
    * Check if some data is set into the instance
    * @return false if instance contains differing data and true if not
    */
    bool is_empty()const;

    template <typename T>
    using DiffMember = Optional<std::pair<T, T>>;

    DiffMember<unsigned long long> crhistoryid;/**< first historyid of contact history*/
    DiffMember<unsigned long long> historyid;/**< last historyid of contact history*/
    DiffMember<Nullable<boost::posix_time::ptime>> delete_time; /**< contact delete time in set local zone*/
    DiffMember<std::string> handle;/**< contact handle */
    DiffMember<std::string> roid;/**< registry object identifier of the contact */
    DiffMember<std::string> sponsoring_registrar_handle;/**< registrar administering the contact */
    DiffMember<std::string> create_registrar_handle;/**< registrar that created the contact */
    DiffMember<Nullable<std::string>> update_registrar_handle;/**< registrar which last time changed the contact */
    DiffMember<boost::posix_time::ptime> creation_time;/**< creation time of the contact in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime>> update_time; /**< last update time of the contact in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime>> transfer_time; /**<last transfer time in set local zone*/
    DiffMember<std::string> authinfopw;/**< unused */
    DiffMember<Nullable<std::string>> name ;/**< name of contact person */
    DiffMember<Nullable<std::string>> organization;/**< full trade name of organization */
    DiffMember< Nullable<LibFred::Contact::PlaceAddress>> place;/**< place address of contact */
    DiffMember<Nullable<std::string>> telephone;/**<  telephone number */
    DiffMember<Nullable<std::string>> fax;/**< fax number */
    DiffMember<Nullable<std::string>> email;/**< e-mail address */
    DiffMember<Nullable<std::string>> notifyemail;/**< to this e-mail address will be send message in case of any change in domain or nsset affecting contact */
    DiffMember<Nullable<std::string>> vat;/**< taxpayer identification number */
    DiffMember< Nullable< PersonalIdUnion >> personal_id;/**< type and value of identification number e.g. social security number, identity card number, date of birth */
    DiffMember<bool> disclosename;/**< whether to reveal contact name */
    DiffMember<bool> discloseorganization;/**< whether to reveal organization */
    DiffMember<bool> discloseaddress;/**< whether to reveal address */
    DiffMember<bool> disclosetelephone;/**< whether to reveal phone number */
    DiffMember<bool> disclosefax;/**< whether to reveal fax number */
    DiffMember<bool> discloseemail;/**< whether to reveal email address */
    DiffMember<bool> disclosevat;/**< whether to reveal taxpayer identification number */
    DiffMember<bool> discloseident;/**< whether to reveal unambiguous identification number */
    DiffMember<bool> disclosenotifyemail;/**< whether to reveal notify email */
    DiffMember<unsigned long long> id;/**< id of the contact object*/
    DiffMember<RegistrableObject::Contact::ContactUuid> uuid;/**< uuid of the contact object*/
    DiffMember<RegistrableObject::Contact::ContactHistoryUuid> history_uuid;/**< history uuid of the contact object*/
    DiffMember<LibFred::ContactAddressList> addresses;/**< additional contact addresses */
    DiffMember<Nullable<bool>> warning_letter;/**< contact preference for sending domain expiration letters */
};

/**
 * Diff data of the contact.
 * @param first
 * @param second
 * @return diff of given contact
 */
InfoContactDiff diff_contact_data(const InfoContactData& first, const InfoContactData& second);

}//namespace LibFred

#endif//INFO_CONTACT_DIFF_HH_741D8D1B6DF14EE1A97088C2CEE33E01
