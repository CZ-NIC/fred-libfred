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
#include "libfred/notifier/gather_email_data/objecttype_specific_impl/domain.hh"

#include "libfred/notifier/gather_email_data/objecttype_specific_impl/util.hh"
#include "libfred/notifier/util/add_old_new_suffix_pair.hh"
#include "libfred/notifier/util/get_previous_object_historyid.hh"
#include "libfred/notifier/util/string_list_utils.hh"
#include "libfred/notifier/util/bool_to_string.hh"
#include "libfred/notifier/util/boost_date_to_cz_string.hh"
#include "libfred/notifier/exception.hh"
#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/keyset/info_keyset.hh"
#include "libfred/registrable_object/domain/info_domain_diff.hh"

#include <boost/algorithm/string/join.hpp>

namespace Notification {

namespace {

std::map<std::string, std::string> gather_domain_update_data_change(
    const LibFred::InfoDomainData& _before,
    const LibFred::InfoDomainData& _after)
{
    std::map<std::string, std::string> result;

    const LibFred::InfoDomainDiff diff = diff_domain_data(_before, _after);

    if (diff.authinfopw.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "object.authinfo",
            diff.authinfopw.get_value().first,
            diff.authinfopw.get_value().second);
    }

    if (diff.registrant.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.registrant",
            diff.registrant.get_value().first.handle,
            diff.registrant.get_value().second.handle);
    }

    if (diff.nsset.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.nsset",
            diff.nsset.get_value().first.isnull()  ? "" : diff.nsset.get_value().first.get_value().handle,
            diff.nsset.get_value().second.isnull() ? "" : diff.nsset.get_value().second.get_value().handle);
    }

    if (diff.keyset.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.keyset",
            diff.keyset.get_value().first.isnull()  ? "" : diff.keyset.get_value().first.get_value().handle,
            diff.keyset.get_value().second.isnull() ? "" : diff.keyset.get_value().second.get_value().handle);
    }

    if (diff.admin_contacts.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.admin_c",
            boost::algorithm::join(sort(get_handles(diff.admin_contacts.get_value().first)), " "),
            boost::algorithm::join(sort(get_handles(diff.admin_contacts.get_value().second)), " "));
    }

    if (diff.enum_domain_validation.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.val_ex_date",
            to_cz_format(
                diff.enum_domain_validation.get_value().first.get_value_or(
                    LibFred::ENUMValidationExtension(
                            boost::gregorian::date(boost::gregorian::not_a_date_time),
                            false)).validation_expiration),
            to_cz_format(
                diff.enum_domain_validation.get_value().second.get_value_or(
                    LibFred::ENUMValidationExtension(
                            boost::gregorian::date(boost::gregorian::not_a_date_time),
                            false)).validation_expiration));
    }

    if (diff.enum_domain_validation.isset())
    {
        add_old_new_changes_pair_if_different(
            result,
            "domain.publish",
            diff.enum_domain_validation.get_value().first.isnull()
                ? "" : to_string(diff.enum_domain_validation.get_value().first.get_value().publish),
            diff.enum_domain_validation.get_value().second.isnull()
                ? "" : to_string(diff.enum_domain_validation.get_value().second.get_value().publish));
    }

    result["changes"] = result.empty() ? "0" : "1";

    return result;
}

}//namespace Notification::{anonymous}

std::map<std::string, std::string> gather_domain_data_change(
    const LibFred::OperationContext& _ctx,
    const notified_event& _event,
    unsigned long long _history_id_post_change)
{
    if (_event != updated)
    {
        return std::map<std::string, std::string>();
    }
    return gather_domain_update_data_change(
        LibFred::InfoDomainHistoryByHistoryid(
            LibFred::get_previous_object_historyid(
                _ctx,
                _history_id_post_change).get_value_or_throw<ExceptionInvalidUpdateEvent>())
        .exec(_ctx).info_domain_data,
        LibFred::InfoDomainHistoryByHistoryid(_history_id_post_change).exec(_ctx).info_domain_data);
}

namespace {

std::set<unsigned long long> get_ids_of_contacts_accepting_notifications(const LibFred::InfoDomainData& _data)
{
    std::set<unsigned long long> result;
    result.insert(_data.registrant.id);
    for (const auto& contact : _data.admin_contacts)
    {
        result.insert(contact.id);
    }
    return result;
}

}//namespace Notification::{anonymous}

std::set<unsigned long long> gather_contact_ids_to_notify_domain_event(
    const LibFred::OperationContext& _ctx,
    notified_event _event,
    unsigned long long _history_id_after_change)
{
    std::set<unsigned long long> contact_ids = get_ids_of_contacts_accepting_notifications(
            LibFred::InfoDomainHistoryByHistoryid(_history_id_after_change).exec(_ctx).info_domain_data);

    // if there were possibly other old values notify those as well
    if (_event == updated)
    {
        const unsigned long long history_id_before_change =
            LibFred::get_previous_object_historyid(_ctx, _history_id_after_change)
                .get_value_or_throw<ExceptionInvalidUpdateEvent>();
        const std::set<unsigned long long> contacts_accepting_notifications_before_change =
                get_ids_of_contacts_accepting_notifications(
                        LibFred::InfoDomainHistoryByHistoryid(history_id_before_change).exec(_ctx).info_domain_data);
        contact_ids.insert(contacts_accepting_notifications_before_change.begin(),
                           contacts_accepting_notifications_before_change.end());

        const LibFred::InfoDomainDiff diff = diff_domain_data(
            LibFred::InfoDomainHistoryByHistoryid(history_id_before_change).exec(_ctx).info_domain_data,
            LibFred::InfoDomainHistoryByHistoryid(_history_id_after_change).exec(_ctx).info_domain_data);

        if (diff.nsset.isset())
        {
            std::set<unsigned long long> nssets;
            if (!diff.nsset.get_value().first.isnull())
            {
                nssets.insert(diff.nsset.get_value().first.get_value().id);
            }
            if (!diff.nsset.get_value().second.isnull())
            {
                nssets.insert(diff.nsset.get_value().second.get_value().id);
            }

            const boost::posix_time::ptime time_of_change =
                    Notification::get_utc_time_of_event(_ctx, _event, _history_id_after_change);

            for (const auto nsset_id : nssets)
            {
                bool corresponding_nsset_history_state_found = false;

                for (const auto& nsset_history_state : LibFred::InfoNssetHistoryById(nsset_id).exec(_ctx, "UTC"))
                {
                    if ((nsset_history_state.history_valid_from <= time_of_change) &&
                        (time_of_change <= nsset_history_state.history_valid_to.get_value_or(boost::posix_time::pos_infin)))
                    {
                        corresponding_nsset_history_state_found = true;
                        for (const auto& contact : nsset_history_state.info_nsset_data.tech_contacts)
                        {
                            contact_ids.insert(contact.id);
                        }
                        /* continuing search through other history versions - chances are there might be two history states bordering the domain event */
                    }
                }

                if (!corresponding_nsset_history_state_found)
                {
                    throw std::runtime_error("inconsistent data - Nsset that was associated before or "
                                             "after event to domain should exist at that time");
                }
            }
        }

        if (diff.keyset.isset())
        {
            std::set<unsigned long long> keysets;
            if (!diff.keyset.get_value().first.isnull())
            {
                keysets.insert(diff.keyset.get_value().first.get_value().id);
            }
            if (!diff.keyset.get_value().second.isnull())
            {
                keysets.insert(diff.keyset.get_value().second.get_value().id);
            }

            const boost::posix_time::ptime time_of_change =
                    Notification::get_utc_time_of_event(_ctx, _event, _history_id_after_change);

            for (const auto keyset_id : keysets)
            {
                bool corresponding_keyset_history_state_found = false;

                for (const auto& keyset_history_state : LibFred::InfoKeysetHistoryById(keyset_id).exec(_ctx, "UTC"))
                {
                    if ((keyset_history_state.history_valid_from <= time_of_change) &&
                        (time_of_change <= keyset_history_state.history_valid_to.get_value_or(boost::posix_time::pos_infin)))
                    {
                        corresponding_keyset_history_state_found = true;
                        for (const auto& contact : keyset_history_state.info_keyset_data.tech_contacts)
                        {
                            contact_ids.insert(contact.id);
                        }
                        /* continuing search through other history versions - chances are there might be two history states bordering the domain event */
                    }
                }

                if (!corresponding_keyset_history_state_found)
                {
                    throw std::runtime_error("inconsistent data - Keyset that was associated before or "
                                             "after event to domain should exist at that time");
                }
            }
        }
    }

    return contact_ids;
}

}//namespace Notification
