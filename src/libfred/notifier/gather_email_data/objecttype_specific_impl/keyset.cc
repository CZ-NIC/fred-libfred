/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/keyset.hh"

#include "libfred/notifier/util/add_old_new_suffix_pair.hh"
#include "libfred/notifier/util/get_previous_object_historyid.hh"
#include "libfred/notifier/util/string_list_utils.hh"
#include "libfred/notifier/exception.hh"
#include "libfred/registrable_object/keyset/info_keyset.hh"
#include "libfred/registrable_object/keyset/info_keyset_diff.hh"

#include <boost/algorithm/string/join.hpp>

namespace Notification {

namespace {

bool sort_by_key(const LibFred::DnsKey& lhs, const LibFred::DnsKey& rhs)
{
    return lhs.get_key() < rhs.get_key();
}

bool equal(const LibFred::DnsKey& lhs, const LibFred::DnsKey& rhs)
{
    return (lhs.get_alg() == rhs.get_alg()) &&
           (lhs.get_protocol() == rhs.get_protocol()) &&
           (lhs.get_alg() == rhs.get_alg()) &&
           (lhs.get_key() == rhs.get_key());
}

bool equal(const std::vector<LibFred::DnsKey>& lhs, const std::vector<LibFred::DnsKey>& rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    for (std::vector<LibFred::DnsKey>::size_type idx = 0; idx < lhs.size(); ++idx)
    {
        if (!equal(lhs[idx], rhs[idx]))
        {
            return false;
        }
    }
    return true;
}

std::string dns_key_to_string(const LibFred::DnsKey& key)
{
    return
        "("
        "flags: " + std::to_string(key.get_flags()) + " "
        "protocol: " + std::to_string(key.get_protocol()) + " "
        "algorithm: " + std::to_string(key.get_alg()) + " "
        "key: " + key.get_key() +
        ")";
}

std::map<std::string, std::string> gather_keyset_update_data_change(
        const LibFred::InfoKeysetData& _before,
        const LibFred::InfoKeysetData& _after)
{
    std::map<std::string, std::string> result;

    const LibFred::InfoKeysetDiff diff = diff_keyset_data(_before, _after);

    if (diff.authinfopw.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "object.authinfo",
            diff.authinfopw.get_value().first,
            diff.authinfopw.get_value().second);
    }

    if (diff.tech_contacts.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "keyset.tech_c",
            boost::algorithm::join(sort(get_handles(diff.tech_contacts.get_value().first)), " "),
            boost::algorithm::join(sort(get_handles(diff.tech_contacts.get_value().second)), " "));
    }

    if (diff.dns_keys.isset())
    {
        std::vector<LibFred::DnsKey> sorted_keys_old = diff.dns_keys.get_value().first;
        std::sort(sorted_keys_old.begin(), sorted_keys_old.end(), sort_by_key);

        std::vector<LibFred::DnsKey> sorted_keys_new = diff.dns_keys.get_value().second;
        std::sort(sorted_keys_new.begin(), sorted_keys_new.end(), sort_by_key);

        if (!equal(sorted_keys_old, sorted_keys_new))
        {
            result["changes.keyset.dnskey"] = "1";

            for (std::vector<LibFred::DnsKey>::size_type idx = 0; idx < sorted_keys_old.size(); ++idx)
            {
                result["changes.keyset.dnskey.old." + std::to_string(idx)] = dns_key_to_string(sorted_keys_old.at(idx));
            }

            for (std::vector<LibFred::DnsKey>::size_type idx = 0; idx < sorted_keys_new.size(); ++idx)
            {
                result["changes.keyset.dnskey.new." + std::to_string(idx)] = dns_key_to_string(sorted_keys_new.at(idx));
            }
        }
    }

    result["changes"] = result.empty() ? "0" : "1";

    return result;
}

}//namespace Notification::{anonymous}

std::map<std::string, std::string> gather_keyset_data_change(
    const LibFred::OperationContext& _ctx,
    const notified_event& _event,
    unsigned long long _history_id_post_change)
{
    if (_event != updated)
    {
        return std::map<std::string, std::string>();
    }
    return gather_keyset_update_data_change(
            LibFred::InfoKeysetHistoryByHistoryid(
                    LibFred::get_previous_object_historyid(
                            _ctx,
                            _history_id_post_change).get_value_or_throw<ExceptionInvalidUpdateEvent>())
            .exec(_ctx).info_keyset_data,
            LibFred::InfoKeysetHistoryByHistoryid(_history_id_post_change).exec(_ctx).info_keyset_data);
}

namespace {

std::set<unsigned long long> get_ids_of_keysets_accepting_notifications(const LibFred::InfoKeysetData& _data)
{
    std::set<unsigned long long> result;
    for (const auto& contact : _data.tech_contacts)
    {
        result.insert(contact.id);
    }
    return result;
}

}//namespace Notification::{anonymous}

std::set<unsigned long long> gather_contact_ids_to_notify_keyset_event(
    const LibFred::OperationContext& _ctx,
    notified_event _event,
    unsigned long long _history_id_after_change)
{
    std::set<unsigned long long> keyset_ids = get_ids_of_keysets_accepting_notifications(
            LibFred::InfoKeysetHistoryByHistoryid(_history_id_after_change).exec(_ctx).info_keyset_data);

    // if there were possibly other old values notify those as well
    if (_event == updated)
    {
        const std::set<unsigned long long> keysets_accepting_notifications_before_change =
                get_ids_of_keysets_accepting_notifications(
                        LibFred::InfoKeysetHistoryByHistoryid(
                                LibFred::get_previous_object_historyid(
                                        _ctx,
                                        _history_id_after_change).get_value_or_throw<ExceptionInvalidUpdateEvent>())
                        .exec(_ctx).info_keyset_data);

        keyset_ids.insert(keysets_accepting_notifications_before_change.begin(),
                          keysets_accepting_notifications_before_change.end());
    }

    return keyset_ids;
}

}//namespace Notification
