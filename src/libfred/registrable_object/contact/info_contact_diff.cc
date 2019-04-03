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
 * contact info data diff
 */

#include "libfred/registrable_object/contact/info_contact_diff.hh"

#include "util/util.hh"
#include "util/is_equal_optional_nullable.hh"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <string>

namespace LibFred {

InfoContactDiff::InfoContactDiff()
{}

namespace {

template <typename T>
void insert_if_presents(
        const InfoContactDiff::DiffMember<T>& data,
        const std::string& name,
        std::set<std::string>& collector)
{
    if (data.is_set())
    {
        collector.insert(name);
    }
}

}//namespace LibFred::{anonymous}

std::set<std::string> InfoContactDiff::changed_fields() const
{
    std::set<std::string> fields;
    insert_if_presents(crhistoryid, "crhistoryid", fields);
    insert_if_presents(historyid, "historyid", fields);
    insert_if_presents(delete_time, "delete_time", fields);
    insert_if_presents(handle, "handle", fields);
    insert_if_presents(roid, "roid", fields);
    insert_if_presents(sponsoring_registrar_handle, "sponsoring_registrar_handle", fields);
    insert_if_presents(create_registrar_handle, "create_registrar_handle", fields);
    insert_if_presents(update_registrar_handle, "update_registrar_handle", fields);
    insert_if_presents(creation_time, "creation_time", fields);
    insert_if_presents(update_time, "update_time", fields);
    insert_if_presents(transfer_time, "transfer_time", fields);
    insert_if_presents(authinfopw, "authinfopw", fields);
    insert_if_presents(name, "name", fields);
    insert_if_presents(organization, "organization", fields);
    insert_if_presents(place, "place", fields);
    insert_if_presents(telephone, "telephone", fields);
    insert_if_presents(fax, "fax", fields);
    insert_if_presents(email, "email", fields);
    insert_if_presents(notifyemail, "notifyemail", fields);
    insert_if_presents(vat, "vat", fields);
    insert_if_presents(personal_id, "personal_id", fields);
    insert_if_presents(disclosename, "disclosename", fields);
    insert_if_presents(discloseorganization, "discloseorganization", fields);
    insert_if_presents(discloseaddress, "discloseaddress", fields);
    insert_if_presents(disclosetelephone, "disclosetelephone", fields);
    insert_if_presents(disclosefax, "disclosefax", fields);
    insert_if_presents(discloseemail, "discloseemail", fields);
    insert_if_presents(disclosevat, "disclosevat", fields);
    insert_if_presents(discloseident, "discloseident", fields);
    insert_if_presents(disclosenotifyemail, "disclosenotifyemail", fields);
    insert_if_presents(id, "id", fields);
    insert_if_presents(uuid, "uuid", fields);
    insert_if_presents(history_uuid, "history_uuid", fields);
    insert_if_presents(addresses, "addresses", fields);
    insert_if_presents(warning_letter, "warning_letter", fields);
    return  fields;
}

namespace {

template <typename T>
auto make_named_quoted_data(const std::string& name, const InfoContactDiff::DiffMember<T>& data)
{
    return std::make_pair(name, data.print_quoted());
}

}//namespace LibFred::{anonymous}

std::string InfoContactDiff::to_string() const
{
    return Util::format_data_structure(
            "InfoContactDiff",
            {
                make_named_quoted_data("crhistoryid", crhistoryid),
                make_named_quoted_data("historyid", historyid),
                make_named_quoted_data("delete_time", delete_time),
                make_named_quoted_data("handle", handle),
                make_named_quoted_data("roid", roid),
                make_named_quoted_data("sponsoring_registrar_handle", sponsoring_registrar_handle),
                make_named_quoted_data("create_registrar_handle", create_registrar_handle),
                make_named_quoted_data("update_registrar_handle", update_registrar_handle),
                make_named_quoted_data("creation_time", creation_time),
                make_named_quoted_data("update_time", update_time),
                make_named_quoted_data("transfer_time", transfer_time),
                make_named_quoted_data("authinfopw", authinfopw),
                make_named_quoted_data("name", name),
                make_named_quoted_data("organization", organization),
                make_named_quoted_data("place", place),
                make_named_quoted_data("telephone", telephone),
                make_named_quoted_data("fax", fax),
                make_named_quoted_data("email", email),
                make_named_quoted_data("notifyemail", notifyemail),
                make_named_quoted_data("vat", vat),
                make_named_quoted_data("personal_id", personal_id),
                make_named_quoted_data("disclosename", disclosename),
                make_named_quoted_data("discloseorganization", discloseorganization),
                make_named_quoted_data("discloseaddress", discloseaddress),
                make_named_quoted_data("disclosetelephone", disclosetelephone),
                make_named_quoted_data("disclosefax", disclosefax),
                make_named_quoted_data("discloseemail", discloseemail),
                make_named_quoted_data("disclosevat", disclosevat),
                make_named_quoted_data("discloseident", discloseident),
                make_named_quoted_data("disclosenotifyemail", disclosenotifyemail),
                make_named_quoted_data("id", id),
                make_named_quoted_data("uuid", uuid),
                make_named_quoted_data("history_uuid", history_uuid),
                make_named_quoted_data("addresses", addresses),
                make_named_quoted_data("warning_letter", warning_letter)
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

bool InfoContactDiff::is_empty() const
{
    return !is_set_any_of(
            crhistoryid,
            historyid,
            delete_time,
            handle,
            roid,
            sponsoring_registrar_handle,
            create_registrar_handle,
            update_registrar_handle,
            creation_time,
            update_time,
            transfer_time,
            authinfopw,
            name,
            organization,
            place,
            telephone,
            fax,
            email,
            notifyemail,
            vat,
            personal_id,
            disclosename,
            discloseorganization,
            discloseaddress,
            disclosetelephone,
            disclosefax,
            discloseemail,
            disclosevat,
            discloseident,
            disclosenotifyemail,
            id,
            uuid,
            history_uuid,
            addresses,
            warning_letter);
}

namespace {

bool operator==(const ContactAddressList&, const ContactAddressList&);
bool operator!=(const ContactAddressList &a, const ContactAddressList &b) { return !(a == b); }

}//namespace LibFred::{anonymous}

InfoContactDiff diff_contact_data(const InfoContactData& first, const InfoContactData& second)
{
    LibFred::InfoContactDiff diff;

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

    if (boost::algorithm::to_upper_copy(first.handle) != boost::algorithm::to_upper_copy(second.handle))
    {
        diff.handle = std::make_pair(first.handle, second.handle);
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

    if (first.authinfopw != second.authinfopw)
    {
        diff.authinfopw = std::make_pair(first.authinfopw, second.authinfopw);
    }

    if (!Util::is_equal(first.name, second.name))
    {
        diff.name = std::make_pair(first.name, second.name);
    }

    if (!Util::is_equal(first.organization, second.organization))
    {
        diff.organization = std::make_pair(first.organization, second.organization);
    }

    if (!Util::is_equal(first.place, second.place))
    {
        diff.place = std::make_pair(first.place, second.place);
    }

    if (!Util::is_equal(first.telephone, second.telephone))
    {
        diff.telephone = std::make_pair(first.telephone, second.telephone);
    }

    if (!Util::is_equal(first.fax, second.fax))
    {
        diff.fax = std::make_pair(first.fax, second.fax);
    }

    if (!Util::is_equal(first.email, second.email))
    {
        diff.email = std::make_pair(first.email, second.email);
    }

    if (!Util::is_equal(first.notifyemail, second.notifyemail))
    {
        diff.notifyemail = std::make_pair(first.notifyemail, second.notifyemail);
    }

    if (!Util::is_equal(first.vat, second.vat))
    {
        diff.vat = std::make_pair(first.vat, second.vat);
    }

    if (!Util::is_equal(first.ssntype, second.ssntype) ||
        !Util::is_equal(first.ssn, second.ssn))
    {
        const Nullable<PersonalIdUnion> a = first.ssntype.isnull() || first.ssn.isnull()
                ? Nullable<PersonalIdUnion>()
                : Nullable<PersonalIdUnion>(
                      PersonalIdUnion::get_any_type(first.ssntype.get_value(), first.ssn.get_value()));
        const Nullable<PersonalIdUnion> b = second.ssntype.isnull() || second.ssn.isnull()
                ? Nullable<PersonalIdUnion>()
                : Nullable<PersonalIdUnion>(
                      PersonalIdUnion::get_any_type(second.ssntype.get_value(), second.ssn.get_value()));
        diff.personal_id = std::make_pair(a, b);
    }

    if (first.disclosename != second.disclosename)
    {
        diff.disclosename = std::make_pair(first.disclosename, second.disclosename);
    }

    if (first.discloseorganization != second.discloseorganization)
    {
        diff.discloseorganization = std::make_pair(first.discloseorganization, second.discloseorganization);
    }

    if (first.discloseaddress != second.discloseaddress)
    {
        diff.discloseaddress = std::make_pair(first.discloseaddress, second.discloseaddress);
    }

    if (first.disclosetelephone != second.disclosetelephone)
    {
        diff.disclosetelephone = std::make_pair(first.disclosetelephone, second.disclosetelephone);
    }

    if (first.disclosefax != second.disclosefax)
    {
        diff.disclosefax = std::make_pair(first.disclosefax, second.disclosefax);
    }

    if (first.discloseemail != second.discloseemail)
    {
        diff.discloseemail = std::make_pair(first.discloseemail, second.discloseemail);
    }

    if (first.disclosevat != second.disclosevat)
    {
        diff.disclosevat = std::make_pair(first.disclosevat, second.disclosevat);
    }

    if (first.discloseident != second.discloseident)
    {
        diff.discloseident = std::make_pair(first.discloseident, second.discloseident);
    }

    if (first.disclosenotifyemail != second.disclosenotifyemail)
    {
        diff.disclosenotifyemail = std::make_pair(first.disclosenotifyemail, second.disclosenotifyemail);
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

    if (first.addresses != second.addresses)
    {
        diff.addresses = std::make_pair(first.addresses, second.addresses);
    }

    if (!Util::is_equal(first.warning_letter, second.warning_letter))
    {
        diff.warning_letter = std::make_pair(first.warning_letter, second.warning_letter);
    }

    return diff;
}

namespace {

bool operator==(const ContactAddressList &lhs, const ContactAddressList &rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }
    ContactAddressList::const_iterator lhs_iter = lhs.begin();
    ContactAddressList::const_iterator rhs_iter = rhs.begin();
    while (true)
    {
        if (lhs_iter == lhs.end())
        {
            return rhs_iter == rhs.end();
        }
        if (rhs_iter == rhs.end())
        {
            return false;
        }
        if ((lhs_iter->first != rhs_iter->first) ||
            (lhs_iter->second != rhs_iter->second))
        {
            return false;
        }
        ++lhs_iter;
        ++rhs_iter;
    }
}

}//namespace LibFred::{anonymous}

}//namespace LibFred
