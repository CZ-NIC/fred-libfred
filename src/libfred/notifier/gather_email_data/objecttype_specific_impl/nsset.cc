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

#include "libfred/notifier/gather_email_data/objecttype_specific_impl/nsset.hh"

#include "libfred/notifier/util/add_old_new_suffix_pair.hh"
#include "libfred/notifier/util/get_previous_object_historyid.hh"
#include "libfred/notifier/util/string_list_utils.hh"
#include "libfred/notifier/exception.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset_diff.hh"

#include <algorithm>
#include <boost/algorithm/string/join.hpp>

namespace Notification {

namespace {

bool sort_by_hostname(const LibFred::DnsHost& lhs, const LibFred::DnsHost& rhs)
{
    return lhs.get_fqdn() < rhs.get_fqdn();
}

bool equal(const LibFred::DnsHost& lhs, const LibFred::DnsHost& rhs)
{
    return (lhs.get_fqdn() == rhs.get_fqdn()) &&
           (lhs.get_inet_addr() == rhs.get_inet_addr());
}

bool equal(const std::vector<LibFred::DnsHost>& lhs, const std::vector<LibFred::DnsHost>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    for (std::vector<LibFred::DnsHost>::size_type idx = 0; idx < lhs.size(); ++idx)
    {
        if (!equal(lhs[idx], rhs[idx]))
        {
            return false;
        }
    }
    return true;
}

std::string dns_host_to_string(const LibFred::DnsHost& lhs)
{
    std::vector<boost::asio::ip::address> sorted_ip_addresses = lhs.get_inet_addr();
    std::sort(sorted_ip_addresses.begin(), sorted_ip_addresses.end()); // boost::asio::address has operator< in sense of represented integer number

    return  lhs.get_fqdn() +
            (sorted_ip_addresses.empty()
                ? ""
                : " (" + boost::algorithm::join(get_string_addresses(sorted_ip_addresses), " ") + ")");
}

std::map<std::string, std::string> gather_nsset_update_data_change(
    const LibFred::InfoNssetData& _before,
    const LibFred::InfoNssetData& _after)
{
    std::map<std::string, std::string> result;

    const LibFred::InfoNssetDiff diff = diff_nsset_data(_before, _after);

    if (diff.tech_check_level.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "nsset.check_level",
            diff.tech_check_level.get_value().first.isnull()
                ? ""
                : std::to_string(diff.tech_check_level.get_value().first.get_value()),
            diff.tech_check_level.get_value().second.isnull()
                ? ""
                : std::to_string(diff.tech_check_level.get_value().second.get_value()));
    }

    if (diff.tech_contacts.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "nsset.tech_c",
            boost::algorithm::join(sort(get_handles(diff.tech_contacts.get_value().first)), " "),
            boost::algorithm::join(sort(get_handles(diff.tech_contacts.get_value().second)), " "));
    }

    if (diff.dns_hosts.isset())
    {
        std::vector<LibFred::DnsHost> sorted_nameservers_old = diff.dns_hosts.get_value().first;
        std::sort(sorted_nameservers_old.begin(), sorted_nameservers_old.end(), sort_by_hostname);

        std::vector<LibFred::DnsHost> sorted_nameservers_new = diff.dns_hosts.get_value().second;
        std::sort(sorted_nameservers_new.begin(), sorted_nameservers_new.end(), sort_by_hostname);

        if (!equal(sorted_nameservers_old, sorted_nameservers_new))
        {
            result["changes.nsset.dns"] = "1";

            for (std::vector<LibFred::DnsHost>::size_type idx = 0; idx < sorted_nameservers_old.size(); ++idx)
            {
                result["changes.nsset.dns.old." + std::to_string(idx)] = dns_host_to_string(sorted_nameservers_old[idx]);
            }

            for (std::vector<LibFred::DnsHost>::size_type idx = 0; idx < sorted_nameservers_new.size(); ++idx)
            {
                result["changes.nsset.dns.new." + std::to_string(idx)] = dns_host_to_string(sorted_nameservers_new[idx]);
            }
        }
    }

    result["changes"] = result.empty() ? "0" : "1";

    return result;
}

}//namespace Notification::{anonymous}

std::map<std::string, std::string> gather_nsset_data_change(
    const LibFred::OperationContext& _ctx,
    notified_event _event,
    unsigned long long _history_id_post_change)
{
    if (_event != updated)
    {
        return std::map<std::string, std::string>();
    }
    return gather_nsset_update_data_change(
            LibFred::InfoNssetHistoryByHistoryid(
                    LibFred::get_previous_object_historyid(
                            _ctx,
                            _history_id_post_change).get_value_or_throw<ExceptionInvalidUpdateEvent>())
            .exec(_ctx).info_nsset_data,
            LibFred::InfoNssetHistoryByHistoryid(_history_id_post_change).exec(_ctx).info_nsset_data);
}

namespace {

std::set<unsigned long long> get_ids_of_nssets_accepting_notifications(const LibFred::InfoNssetData& _data)
{
    std::set<unsigned long long> result;
    for (const auto& contact : _data.tech_contacts)
    {
        result.insert(contact.id);
    }
    return result;
}

}//namespace Notification::{anonymous}

std::set<unsigned long long> gather_contact_ids_to_notify_nsset_event(
    const LibFred::OperationContext& _ctx,
    notified_event _event,
    unsigned long long _history_id_after_change)
{
    std::set<unsigned long long> nsset_ids = get_ids_of_nssets_accepting_notifications(
            LibFred::InfoNssetHistoryByHistoryid(_history_id_after_change).exec(_ctx).info_nsset_data);

    // if there were possibly other old values notify those as well
    if (_event == updated)
    {
        const std::set<unsigned long long> nssets_accepting_notifications_before_change =
                get_ids_of_nssets_accepting_notifications(
                        LibFred::InfoNssetHistoryByHistoryid(
                                LibFred::get_previous_object_historyid(
                                        _ctx,
                                        _history_id_after_change)
                                .get_value_or_throw<ExceptionInvalidUpdateEvent>())
                        .exec(_ctx).info_nsset_data);

        nsset_ids.insert(nssets_accepting_notifications_before_change.begin(),
                         nssets_accepting_notifications_before_change.end());
    }
    return nsset_ids;
}

}//namespace Notification
