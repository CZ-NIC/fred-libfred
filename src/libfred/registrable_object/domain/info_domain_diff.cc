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

#include "libfred/registrable_object/domain/info_domain_diff.hh"
#include "util/util.hh"
#include "util/is_equal_optional_nullable.hh"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <string>

namespace LibFred {

InfoDomainDiff::InfoDomainDiff()
{}

namespace {

template <typename T>
void insert_if_presents(
        const InfoDomainDiff::DiffMember<T>& data,
        const std::string& name,
        std::set<std::string>& collector)
{
    if (data.is_set())
    {
        collector.insert(name);
    }
}

}//namespace LibFred::{anonymous}

std::set<std::string> InfoDomainDiff::changed_fields() const
{
    std::set<std::string> fields;
    insert_if_presents(crhistoryid, "crhistoryid", fields);
    insert_if_presents(historyid, "historyid", fields);
    insert_if_presents(delete_time, "delete_time", fields);
    insert_if_presents(fqdn, "fqdn", fields);
    insert_if_presents(roid, "roid", fields);
    insert_if_presents(sponsoring_registrar_handle, "sponsoring_registrar_handle", fields);
    insert_if_presents(create_registrar_handle, "create_registrar_handle", fields);
    insert_if_presents(update_registrar_handle, "update_registrar_handle", fields);
    insert_if_presents(creation_time, "creation_time", fields);
    insert_if_presents(update_time, "update_time", fields);
    insert_if_presents(transfer_time, "transfer_time", fields);

    insert_if_presents(registrant, "registrant", fields);
    insert_if_presents(nsset, "nsset", fields);
    insert_if_presents(keyset, "keyset", fields);
    insert_if_presents(expiration_date, "expiration_date", fields);
    insert_if_presents(admin_contacts, "admin_contacts", fields);
    insert_if_presents(enum_domain_validation, "enum_domain_validation", fields);
    insert_if_presents(zone, "zone", fields);

    insert_if_presents(id, "id", fields);
    insert_if_presents(uuid, "uuid", fields);
    insert_if_presents(history_uuid, "history_uuid", fields);

    return fields;
}

namespace {

template <typename T>
auto make_named_quoted_data(const std::string& name, const InfoDomainDiff::DiffMember<T>& data)
{
    return std::make_pair(name, data.print_quoted());
}

}//namespace LibFred::{anonymous}

std::string InfoDomainDiff::to_string() const
{
    return Util::format_data_structure(
            "InfoDomainDiff",
            {
                make_named_quoted_data("crhistoryid", crhistoryid),
                make_named_quoted_data("historyid", historyid),
                make_named_quoted_data("delete_time", delete_time),
                make_named_quoted_data("fqdn", fqdn),
                make_named_quoted_data("roid", roid),
                make_named_quoted_data("sponsoring_registrar_handle", sponsoring_registrar_handle),
                make_named_quoted_data("create_registrar_handle", create_registrar_handle),
                make_named_quoted_data("update_registrar_handle", update_registrar_handle),
                make_named_quoted_data("creation_time", creation_time),
                make_named_quoted_data("update_time", update_time),
                make_named_quoted_data("transfer_time", transfer_time),

                make_named_quoted_data("registrant", registrant),
                make_named_quoted_data("nsset", nsset),
                make_named_quoted_data("keyset", keyset),
                make_named_quoted_data("expiration_date", expiration_date),
                make_named_quoted_data("admin_contacts", admin_contacts),
                make_named_quoted_data("enum_domain_validation", enum_domain_validation),
                make_named_quoted_data("zone", zone),

                make_named_quoted_data("id", id),
                make_named_quoted_data("uuid", uuid),
                make_named_quoted_data("history_uuid", history_uuid)
            });
}

namespace {

bool is_set_any_of()
{
    return false;
}

template <typename F, typename ...O>
bool is_set_any_of(const Optional<F>& first, const Optional<O>& ...others)
{
    return first.is_set() || is_set_any_of(others...);
}

}//namespace LibFred::{anonymous}

bool InfoDomainDiff::is_empty() const
{
    return !is_set_any_of(
            crhistoryid,
            historyid,
            delete_time,
            fqdn,
            roid,
            sponsoring_registrar_handle,
            create_registrar_handle,
            update_registrar_handle,
            creation_time,
            update_time,
            transfer_time,

            registrant,
            nsset,
            keyset,
            expiration_date,
            admin_contacts,
            enum_domain_validation,
            zone,

            id,
            uuid,
            history_uuid);
}

InfoDomainDiff diff_domain_data(const InfoDomainData& first, const InfoDomainData& second)
{
    LibFred::InfoDomainDiff diff;

    //differing data
    if (first.crhistoryid != second.crhistoryid)
    {
        diff.crhistoryid = std::make_pair(first.crhistoryid, second.crhistoryid);
    }

    if (first.historyid != second.historyid)
    {
        diff.historyid = std::make_pair(first.historyid, second.historyid);
    }

    if (!Util::is_equal(first.delete_time, second.delete_time))
    {
        diff.delete_time = std::make_pair(first.delete_time, second.delete_time);
    }

    if (boost::algorithm::to_lower_copy(first.fqdn) != boost::algorithm::to_lower_copy(second.fqdn))
    {
        diff.fqdn = std::make_pair(first.fqdn, second.fqdn);
    }

    if (first.roid != second.roid)
    {
        diff.roid = std::make_pair(first.roid, second.roid);
    }

    if (boost::algorithm::to_upper_copy(first.sponsoring_registrar_handle) !=
        boost::algorithm::to_upper_copy(second.sponsoring_registrar_handle))
    {
        diff.sponsoring_registrar_handle = std::make_pair(
                first.sponsoring_registrar_handle,
                second.sponsoring_registrar_handle);
    }

    if (boost::algorithm::to_upper_copy(first.create_registrar_handle) !=
        boost::algorithm::to_upper_copy(second.create_registrar_handle))
    {
        diff.create_registrar_handle = std::make_pair(
                first.create_registrar_handle,
                second.create_registrar_handle);
    }

    if (!Util::is_equal_upper(first.update_registrar_handle, second.update_registrar_handle))
    {
        diff.update_registrar_handle = std::make_pair(
                first.update_registrar_handle,
                second.update_registrar_handle);
    }

    if (first.creation_time != second.creation_time)
    {
        diff.creation_time = std::make_pair(first.creation_time, second.creation_time);
    }

    if (!Util::is_equal(first.update_time, second.update_time))
    {
        diff.update_time = std::make_pair(first.update_time, second.update_time);
    }

    if (!Util::is_equal(first.transfer_time, second.transfer_time))
    {
        diff.transfer_time = std::make_pair(first.transfer_time, second.transfer_time);
    }

    if (first.registrant != second.registrant)
    {
        diff.registrant = std::make_pair(first.registrant, second.registrant);
    }

    if (!Util::is_equal(first.nsset, second.nsset))
    {
        diff.nsset = std::make_pair(first.nsset, second.nsset);
    }

    if (!Util::is_equal(first.keyset, second.keyset))
    {
        diff.keyset = std::make_pair(first.keyset, second.keyset);
    }

    if (first.expiration_date != second.expiration_date)
    {
        diff.expiration_date = std::make_pair(first.expiration_date, second.expiration_date);
    }

    std::set<RegistrableObject::Contact::ContactReference> lhs_admin_contacts;
    for (const auto& contact : first.admin_contacts)
    {
        lhs_admin_contacts.insert(contact);
    }

    std::set<RegistrableObject::Contact::ContactReference> rhs_admin_contacts;
    for (const auto& contact : second.admin_contacts)
    {
        rhs_admin_contacts.insert(contact);
    }

    if (lhs_admin_contacts != rhs_admin_contacts)
    {
        diff.admin_contacts = std::make_pair(first.admin_contacts, second.admin_contacts);
    }

    if (!Util::is_equal(first.enum_domain_validation, second.enum_domain_validation))
    {
        diff.enum_domain_validation = std::make_pair(first.enum_domain_validation, second.enum_domain_validation);
    }

    if (first.zone != second.zone)
    {
        diff.zone = std::make_pair(first.zone, second.zone);
    }

    if (first.id != second.id)
    {
        diff.id = std::make_pair(first.id, second.id);
    }

    if (get_raw_value_from(first.uuid) != get_raw_value_from(second.uuid))
    {
        diff.uuid = std::make_pair(first.uuid, second.uuid);
    }

    if (get_raw_value_from(first.history_uuid) != get_raw_value_from(second.history_uuid))
    {
        diff.history_uuid = std::make_pair(first.history_uuid, second.history_uuid);
    }

    return diff;
}

}//namespace LibFred
