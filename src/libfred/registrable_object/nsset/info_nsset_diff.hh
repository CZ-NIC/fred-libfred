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

#ifndef INFO_NSSET_DIFF_HH_90CCBE2D77FA4C68BEBE583D68FD17E5
#define INFO_NSSET_DIFF_HH_90CCBE2D77FA4C68BEBE583D68FD17E5

#include "libfred/registrable_object/nsset/info_nsset_data.hh"
#include "libfred/registrable_object/contact/contact_reference.hh"

#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <set>
#include <string>
#include <utility>
#include <vector>

namespace LibFred {

/**
 * Diff of nsset data.
 * Data of the nsset difference with the same members as nsset data but in optional pairs. Optional pair member is set in case of difference in compared nsset data.
 */
struct InfoNssetDiff : Util::Printable<InfoNssetDiff>
{
    /**
    * Constructor of the nsset data diff structure.
    */
    InfoNssetDiff();

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

    DiffMember<unsigned long long> crhistoryid;/**< first historyid of nsset history*/
    DiffMember<unsigned long long> historyid;/**< last historyid of nsset history*/
    DiffMember<Nullable<boost::posix_time::ptime> > delete_time; /**< nsset delete time in set local zone*/
    DiffMember<std::string> handle;/**< nsset handle */
    DiffMember<std::string> roid;/**< registry object identifier of the nsset */
    DiffMember<std::string> sponsoring_registrar_handle;/**< registrar administering the nsset */
    DiffMember<std::string> create_registrar_handle;/**< registrar that created the nsset */
    DiffMember<Nullable<std::string> > update_registrar_handle;/**< registrar which last time changed the nsset */
    DiffMember<boost::posix_time::ptime> creation_time;/**< creation time of the nsset in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime> > update_time; /**< last update time of the nsset in set local zone*/
    DiffMember<Nullable<boost::posix_time::ptime> > transfer_time; /**<last transfer time in set local zone*/
    DiffMember<std::string> authinfopw;/**< unused */

    DiffMember<Nullable<short> > tech_check_level; /**< nsset level of technical checks */
    DiffMember<std::vector<DnsHost> > dns_hosts;/**< DNS hosts */
    DiffMember<std::vector<RegistrableObject::Contact::ContactReference>> tech_contacts;/**< list of technical contacts */

    DiffMember<unsigned long long> id;/**< id of the nsset object*/
    DiffMember<RegistrableObject::Nsset::NssetUuid> uuid;/**< uuid of the nsset object*/
    DiffMember<RegistrableObject::Nsset::NssetHistoryUuid> history_uuid;/**< history uuid of the nsset object*/
};

/**
 * Diff data of the nsset.
 * @param first
 * @param second
 * @return diff of given nsset
 */
InfoNssetDiff diff_nsset_data(const InfoNssetData& first, const InfoNssetData& second);

}//namespace LibFred

#endif
