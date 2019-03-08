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
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/nsset.hh"

#include "libfred/notifier/util/add_old_new_suffix_pair.hh"
#include "libfred/notifier/util/get_previous_object_historyid.hh"
#include "libfred/notifier/util/string_list_utils.hh"
#include "libfred/notifier/exception.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset_diff.hh"

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>

namespace Notification {

static bool sort_by_hostname(const LibFred::DnsHost& a, const LibFred::DnsHost& b) {
    return a.get_fqdn() < b.get_fqdn();
}


static bool equal(const LibFred::DnsHost& a, const LibFred::DnsHost& b) {
    return
    a.get_fqdn() == b.get_fqdn()
    &&
    a.get_inet_addr() == b.get_inet_addr();
}

static bool equal(const std::vector<LibFred::DnsHost>& a, const std::vector<LibFred::DnsHost>& b) {
    if (a.size() != b.size() ) {
        return false;
    }
    for (std::vector<LibFred::DnsHost>::size_type i = 0; i < a.size(); ++i) {
        if (! equal(a.at(i), b.at(i)) ) { return false; }
    }
    return true;
}

static std::string dns_host_to_string(const LibFred::DnsHost& a) {
    std::vector<boost::asio::ip::address> sorted_ip_addresses = a.get_inet_addr();
    std::sort(sorted_ip_addresses.begin(), sorted_ip_addresses.end()); // boost::asio::address has operator< in sense of represented integer number

    return  a.get_fqdn() +
            (
                sorted_ip_addresses.empty()
                ?   ""
                :   " (" + boost::algorithm::join( get_string_addresses(sorted_ip_addresses), " ") + ")"
            );
}

static std::map<std::string, std::string> gather_nsset_update_data_change(
    const LibFred::InfoNssetData& _before,
    const LibFred::InfoNssetData& _after
) {
    std::map<std::string, std::string> result;

    const LibFred::InfoNssetDiff diff = diff_nsset_data(_before, _after);

    if (diff.authinfopw.isset()) {
        add_old_new_changes_pair_if_different(
            result, "object.authinfo",
            diff.authinfopw.get_value().first,
            diff.authinfopw.get_value().second
        );
    }

    if (diff.tech_check_level.isset()) {
        add_old_new_changes_pair_if_different(
            result, "nsset.check_level",
            diff.tech_check_level.get_value().first.isnull()
                ? ""
                : boost::lexical_cast<std::string>( diff.tech_check_level.get_value().first.get_value() ),
            diff.tech_check_level.get_value().second.isnull()
                ? ""
                : boost::lexical_cast<std::string>( diff.tech_check_level.get_value().second.get_value() )
        );
    }

    if (diff.tech_contacts.isset()) {
        add_old_new_changes_pair_if_different(
            result, "nsset.tech_c",
            boost::algorithm::join( sort( get_handles( diff.tech_contacts.get_value().first  ) ), " " ),
            boost::algorithm::join( sort( get_handles( diff.tech_contacts.get_value().second ) ), " " )
        );
    }

    if (diff.dns_hosts.isset()) {
        std::vector<LibFred::DnsHost> sorted_nameservers_old = diff.dns_hosts.get_value().first;
        std::sort(sorted_nameservers_old.begin(), sorted_nameservers_old.end(), sort_by_hostname);

        std::vector<LibFred::DnsHost> sorted_nameservers_new = diff.dns_hosts.get_value().second;
        std::sort(sorted_nameservers_new.begin(), sorted_nameservers_new.end(), sort_by_hostname);

        if (! equal(sorted_nameservers_old, sorted_nameservers_new) ) {
            result["changes.nsset.dns"] = "1";

            for (std::vector<LibFred::DnsHost>::size_type i = 0; i < sorted_nameservers_old.size(); ++i) {
                result["changes.nsset.dns.old." + boost::lexical_cast<std::string>(i)] = dns_host_to_string( sorted_nameservers_old.at(i) );
            }

            for (std::vector<LibFred::DnsHost>::size_type i = 0; i < sorted_nameservers_new.size(); ++i) {
                result["changes.nsset.dns.new." + boost::lexical_cast<std::string>(i)] = dns_host_to_string( sorted_nameservers_new.at(i) );
            }
        }
    }

    result["changes"] = result.empty() ? "0" : "1";

    return result;
}

std::map<std::string, std::string> gather_nsset_data_change(
    LibFred::OperationContext& _ctx,
    const notified_event& _event,
    unsigned long long _history_id_post_change
) {

    if (_event != updated ) {

        return std::map<std::string, std::string>();

    } else {

        return gather_nsset_update_data_change(
            LibFred::InfoNssetHistoryByHistoryid(
                LibFred::get_previous_object_historyid(_ctx, _history_id_post_change)
                    .get_value_or_throw<ExceptionInvalidUpdateEvent>()
            ).exec(_ctx).info_nsset_data,
            LibFred::InfoNssetHistoryByHistoryid(_history_id_post_change).exec(_ctx).info_nsset_data
        );
    }

}



static std::set<unsigned long long> get_ids_of_nssets_accepting_notifications(const LibFred::InfoNssetData& _data) {
    std::set<unsigned long long> result;
    BOOST_FOREACH(const LibFred::ObjectIdHandlePair& tech_c, _data.tech_contacts) {
        result.insert(tech_c.id);
    }

    return result;
}

std::set<unsigned long long> gather_contact_ids_to_notify_nsset_event(
    LibFred::OperationContext& _ctx,
    notified_event _event,
    unsigned long long _history_id_after_change
) {
    std::set<unsigned long long> nsset_ids;

    // always notify new values of notifiable contacts
    {
        const std::set<unsigned long long> nssets_accepting_notifications_after_change = get_ids_of_nssets_accepting_notifications(
            LibFred::InfoNssetHistoryByHistoryid(_history_id_after_change).exec(_ctx).info_nsset_data
        );
        nsset_ids.insert(nssets_accepting_notifications_after_change.begin(), nssets_accepting_notifications_after_change.end());
    }

    // if there were possibly other old values notify those as well
    if (_event == updated ) {
        const std::set<unsigned long long> nssets_accepting_notifications_before_change = get_ids_of_nssets_accepting_notifications(
            LibFred::InfoNssetHistoryByHistoryid(
                LibFred::get_previous_object_historyid(_ctx, _history_id_after_change)
                    .get_value_or_throw<ExceptionInvalidUpdateEvent>()
            ).exec(_ctx).info_nsset_data
        );

        nsset_ids.insert( nssets_accepting_notifications_before_change.begin(), nssets_accepting_notifications_before_change.end() );
    }

    return nsset_ids;
}

}
