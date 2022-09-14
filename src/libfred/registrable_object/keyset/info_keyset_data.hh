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

#ifndef INFO_KEYSET_DATA_HH_FF8C3982D4304BADBA11BD995610CB24
#define INFO_KEYSET_DATA_HH_FF8C3982D4304BADBA11BD995610CB24

#include "util/db/nullable.hh"
#include "util/printable.hh"

#include "libfred/registrable_object/contact/contact_reference.hh"
#include "libfred/registrable_object/keyset/keyset_dns_key.hh"
#include "libfred/registrable_object/keyset/keyset_uuid.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>

namespace LibFred {

/**
 * Common data of keyset.
 * Current or history state of the keyset.
 */
struct InfoKeysetData : Util::Printable<InfoKeysetData>
{
    /**
     * Constructor of the keyset data structure.
     */
    InfoKeysetData();

    /**
     * Equality of the keyset data structure operator.
     * @param rhs is right hand side of the keyset data comparison
     * @return true if equal, false if not
     */
    bool operator==(const InfoKeysetData& rhs) const;

    /**
     * Inequality of the keyset data structure operator.
     * @param rhs is right hand side of the keyset data comparison
     * @return true if not equal, false if equal
     */
    bool operator!=(const InfoKeysetData& rhs) const;

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string()const;

    unsigned long long crhistoryid;/**< first historyid of keyset history */
    unsigned long long historyid;/**< last historyid of keyset history */
    unsigned long long id;/**< id of the keyset object*/
    RegistrableObject::Keyset::KeysetUuid uuid;/**< uuid of the keyset object*/
    RegistrableObject::Keyset::KeysetHistoryUuid history_uuid;/**< last history_uuid of keyset history */
    Nullable<boost::posix_time::ptime> delete_time; /**< keyset delete time in local time zone viz @ref local_timestamp_pg_time_zone_name */
    std::string handle;/**< keyset handle */
    std::string roid;/**< registry object identifier of the keyset */
    std::string sponsoring_registrar_handle;/**< registrar administering the keyset */
    std::string create_registrar_handle;/**< registrar that created the keyset */
    Nullable<std::string> update_registrar_handle;/**< registrar which last time changed the keyset */
    boost::posix_time::ptime creation_time;/**< creation time of the keyset in local time zone viz @ref local_timestamp_pg_time_zone_name*/
    Nullable<boost::posix_time::ptime> update_time; /**< last update time of the keyset in local time zone viz @ref local_timestamp_pg_time_zone_name*/
    Nullable<boost::posix_time::ptime> transfer_time; /**<last transfer time in local time zone viz @ref local_timestamp_pg_time_zone_name*/
    std::string authinfopw;/**< unused */
    std::vector<DnsKey> dns_keys;/**< DNS keys */
    std::vector<RegistrableObject::Contact::ContactReference> tech_contacts;/**< list of technical contact handles */
};

}//namespace LibFred

#endif//INFO_KEYSET_DATA_HH_FF8C3982D4304BADBA11BD995610CB24
