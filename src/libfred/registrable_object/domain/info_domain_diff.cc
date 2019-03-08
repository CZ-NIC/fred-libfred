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
 * domain info data diff
 */

#include <algorithm>
#include <string>
#include <vector>
#include <set>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "util/util.hh"
#include "util/is_equal_optional_nullable.hh"
#include "libfred/registrable_object/domain/info_domain_diff.hh"

namespace LibFred
{
    InfoDomainDiff::InfoDomainDiff()
    {}

    std::set<std::string> InfoDomainDiff::changed_fields() const
    {
        std::set<std::string> fields;
        if (crhistoryid.isset()) fields.insert("crhistoryid");
        if (historyid.isset()) fields.insert("historyid");
        if (delete_time.isset()) fields.insert("delete_time");
        if (fqdn.isset()) fields.insert("fqdn");
        if (roid.isset()) fields.insert("roid");
        if (sponsoring_registrar_handle.isset()) fields.insert("sponsoring_registrar_handle");
        if (create_registrar_handle.isset()) fields.insert("create_registrar_handle");
        if (update_registrar_handle.isset()) fields.insert("update_registrar_handle");
        if (creation_time.isset()) fields.insert("creation_time");
        if (update_time.isset()) fields.insert("update_time");
        if (transfer_time.isset()) fields.insert("transfer_time");
        if (authinfopw.isset()) fields.insert("authinfopw");

        if (registrant.isset()) fields.insert("registrant");
        if (nsset.isset()) fields.insert("nsset");
        if (keyset.isset()) fields.insert("keyset");
        if (expiration_date.isset()) fields.insert("expiration_date");
        if (admin_contacts.isset()) fields.insert("admin_contacts");
        if (enum_domain_validation.isset()) fields.insert("enum_domain_validation");
        if (zone.isset()) fields.insert("zone");

        if (id.isset()) fields.insert("id");

        return fields;
    }

    std::string InfoDomainDiff::to_string() const
    {
        return Util::format_data_structure("InfoDomainDiff",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("crhistoryid", crhistoryid.print_quoted()))
        (std::make_pair("historyid", historyid.print_quoted()))
        (std::make_pair("delete_time", delete_time.print_quoted()))
        (std::make_pair("fqdn", fqdn.print_quoted()))
        (std::make_pair("roid", roid.print_quoted()))
        (std::make_pair("sponsoring_registrar_handle", sponsoring_registrar_handle.print_quoted()))
        (std::make_pair("create_registrar_handle", create_registrar_handle.print_quoted()))
        (std::make_pair("update_registrar_handle", update_registrar_handle.print_quoted()))
        (std::make_pair("creation_time", creation_time.print_quoted()))
        (std::make_pair("update_time", update_time.print_quoted()))
        (std::make_pair("transfer_time", transfer_time.print_quoted()))
        (std::make_pair("authinfopw", authinfopw.print_quoted()))

        (std::make_pair("registrant", registrant.print_quoted()))
        (std::make_pair("nsset", nsset.print_quoted()))
        (std::make_pair("keyset", keyset.print_quoted()))
        (std::make_pair("expiration_date", expiration_date.print_quoted()))
        (std::make_pair("admin_contacts", admin_contacts.print_quoted()))
        (std::make_pair("enum_domain_validation", enum_domain_validation.print_quoted()))
        (std::make_pair("zone", zone.print_quoted()))

        (std::make_pair("id", id.print_quoted()))
        );//format_data_structure InfoDomainDiff
    }

    bool InfoDomainDiff::is_empty() const
    {
        return
            !( crhistoryid.isset()
            || historyid.isset()
            || delete_time.isset()
            || fqdn.isset()
            || roid.isset()
            || sponsoring_registrar_handle.isset()
            || create_registrar_handle.isset()
            || update_registrar_handle.isset()
            || creation_time.isset()
            || update_time.isset()
            || transfer_time.isset()
            || authinfopw.isset()

            || registrant.isset()
            || nsset.isset()
            || keyset.isset()
            || expiration_date.isset()
            || admin_contacts.isset()
            || enum_domain_validation.isset()
            || zone.isset()

            || id.isset()
            );
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

        if (boost::algorithm::to_lower_copy(first.fqdn).compare(boost::algorithm::to_lower_copy(second.fqdn)) != 0)
        {
            diff.fqdn = std::make_pair(first.fqdn, second.fqdn);
        }

        if (first.roid.compare(second.roid) != 0)
        {
            diff.roid = std::make_pair(first.roid, second.roid);
        }

        if (boost::algorithm::to_upper_copy(first.sponsoring_registrar_handle)
            .compare(boost::algorithm::to_upper_copy(second.sponsoring_registrar_handle)) != 0)
        {
            diff.sponsoring_registrar_handle = std::make_pair(first.sponsoring_registrar_handle
                    , second.sponsoring_registrar_handle);
        }

        if (boost::algorithm::to_upper_copy(first.create_registrar_handle)
        .compare(boost::algorithm::to_upper_copy(second.create_registrar_handle)) != 0)
        {
            diff.create_registrar_handle = std::make_pair(first.create_registrar_handle
                    , second.create_registrar_handle);
        }

        if (!Util::is_equal_upper(first.update_registrar_handle, second.update_registrar_handle))
        {
            diff.update_registrar_handle = std::make_pair(first.update_registrar_handle
                    , second.update_registrar_handle);
        }

        if (first.creation_time != second.creation_time)
        {
            diff.creation_time = std::make_pair(first.creation_time, second.creation_time);
        }

        if (!Util::is_equal(first.update_time, second.update_time))
        {
            diff.update_time = std::make_pair(first.update_time
                    , second.update_time);
        }

        if (!Util::is_equal(first.transfer_time, second.transfer_time))
        {
            diff.transfer_time = std::make_pair(first.transfer_time
                    , second.transfer_time);
        }

        if (first.authinfopw.compare(second.authinfopw) != 0)
        {
            diff.authinfopw = std::make_pair(first.authinfopw, second.authinfopw);
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

        std::set<ObjectIdHandlePair> lhs_admin_contacts;
        for (std::vector<ObjectIdHandlePair>::size_type i = 0
            ; i != first.admin_contacts.size(); ++i)
        {
            lhs_admin_contacts.insert(first.admin_contacts[i]);
        }

        std::set<ObjectIdHandlePair> rhs_admin_contacts;
        for (std::vector<ObjectIdHandlePair>::size_type i = 0
            ; i != second.admin_contacts.size(); ++i)
        {
            rhs_admin_contacts.insert(second.admin_contacts[i]);
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

        return diff;
    }

} // namespace LibFred
