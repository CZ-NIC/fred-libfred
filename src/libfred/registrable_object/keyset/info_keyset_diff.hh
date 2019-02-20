/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
 *  keyset info data diff
 */

#ifndef INFO_KEYSET_DIFF_HH_5166F7638312403697651744C3726161
#define INFO_KEYSET_DIFF_HH_5166F7638312403697651744C3726161

#include "libfred/registrable_object/contact/contact_reference.hh"
#include "libfred/registrable_object/keyset/info_keyset_data.hh"

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>
#include <set>
#include <utility>

namespace LibFred {

/**
 * Diff of keyset data.
 * Data of the keyset difference with the same members as keyset data but in optional pairs. Optional pair member is set in case of difference in compared keyset data.
 */
struct InfoKeysetDiff : Util::Printable<InfoKeysetDiff>
{
    /**
    * Constructor of the keyset data diff structure.
    */
    InfoKeysetDiff();

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

    template <class T> struct DiffMemeber { typedef Optional<std::pair<T, T>> Type;};

    DiffMemeber<unsigned long long>::Type crhistoryid;/**< first historyid of keyset history*/
    DiffMemeber<unsigned long long>::Type historyid;/**< last historyid of keyset history*/
    DiffMemeber<Nullable<boost::posix_time::ptime> >::Type delete_time; /**< keyset delete time in set local zone*/
    DiffMemeber<std::string>::Type handle;/**< keyset handle */
    DiffMemeber<std::string>::Type roid;/**< registry object identifier of the keyset */
    DiffMemeber<std::string>::Type sponsoring_registrar_handle;/**< registrar administering the keyset */
    DiffMemeber<std::string>::Type create_registrar_handle;/**< registrar that created the keyset */
    DiffMemeber<Nullable<std::string> >::Type update_registrar_handle;/**< registrar which last time changed the keyset */
    DiffMemeber<boost::posix_time::ptime>::Type creation_time;/**< creation time of the keyset in set local zone*/
    DiffMemeber<Nullable<boost::posix_time::ptime> >::Type update_time; /**< last update time of the keyset in set local zone*/
    DiffMemeber<Nullable<boost::posix_time::ptime> >::Type transfer_time; /**<last transfer time in set local zone*/
    DiffMemeber<std::string>::Type authinfopw;/**< password for transfer */

    DiffMemeber<std::vector<DnsKey> >::Type dns_keys;/**< DNS keys */
    DiffMemeber<std::vector<RegistrableObject::Contact::ContactReference>>::Type tech_contacts;/**< list of technical contacts*/

    DiffMemeber<unsigned long long>::Type id;/**< id of the keyset object*/
};

/**
 * Diff data of the keyset.
 * @param first
 * @param second
 * @return diff of given keyset
 */
InfoKeysetDiff diff_keyset_data(const InfoKeysetData& first, const InfoKeysetData& second);

}//namespace LibFred

#endif//INFO_KEYSET_DIFF_HH_5166F7638312403697651744C3726161
