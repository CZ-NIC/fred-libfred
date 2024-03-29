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
/**
 *  @merge_contact_selection.cc
 *  selection of contact for merging
 */

#include "libfred/registrable_object/contact/merge_contact_selection.hh"
#include "libfred/object/object_state.hh"
#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"

#include "util/util.hh"

#include <boost/regex.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace LibFred {

MergeContactSelectionOutput::MergeContactSelectionOutput(std::string _handle, std::string _filter)
    : handle{std::move(_handle)},
      filter{std::move(_filter)}
{}

std::string MergeContactSelectionOutput::to_string() const
{
    return Util::format_data_structure(
            "MergeContactSelectionOutput",
            { std::make_pair(std::string{"handle"}, handle),
              std::make_pair(std::string{"filter"}, filter) });
}

MergeContactSelection::MergeContactSelection(
        std::vector<std::string> contact_handles,
        std::vector<std::string> filters)
    : contact_handles_{std::move(contact_handles)},
      filters_{std::move(filters)}
{ }

MergeContactSelectionOutput MergeContactSelection::exec(const OperationContext& ctx)
{
    try
    {
        if (contact_handles_.empty())
        {
            BOOST_THROW_EXCEPTION(NoContactHandles());
        }
        const auto& filter = get_default_contact_selection_filter_factory();
        for (auto&& filter_name : filters_)
        {
            if (filter.has_key(filter_name))
            {
                auto current_filter_result = filter[filter_name](ctx, contact_handles_);
                if (current_filter_result.size() == 1)
                {
                    return MergeContactSelectionOutput{current_filter_result[0], filter_name};
                }
                if (1 < current_filter_result.size())
                {
                    contact_handles_ = std::move(current_filter_result);
                }
            }
        }

        if (contact_handles_.empty())
        {
            BOOST_THROW_EXCEPTION(NoContactHandlesLeft{});
        }
        BOOST_THROW_EXCEPTION(TooManyContactHandlesLeft{});
    }
    catch (ExceptionStack& ex)
    {
        ex.add_exception_stack_info(to_string());
        throw;
    }
}

std::string MergeContactSelection::to_string() const
{
    std::vector<std::pair<std::string, std::string>> data;
    data.push_back((std::make_pair("contact_handle", Util::format_container(contact_handles_))));
    std::ostringstream os;
    std::for_each(begin(filters_), end(filters_), [&](auto&& filter_name) { os << " " << filter_name; });
    data.push_back((std::make_pair("selection filters", os.str())));
    return Util::format_operation_state("MergeContactSelection", data);
}

const char* MergeContactSelection::NoContactHandles::what() const noexcept { return "no contact handles, nothing to process"; }
const char* MergeContactSelection::NoContactHandlesLeft::what() const noexcept { return "no contact handles left, selection of contact with given rules failed"; }
const char* MergeContactSelection::TooManyContactHandlesLeft::what() const noexcept { return "too many contact handles left, selection of contact with given rules failed"; }

namespace {

class FilterIdentifiedContact : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(const OperationContext& ctx, const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            const Database::Result contact_check = ctx.get_conn().exec_params(
                "SELECT oreg.name FROM contact c JOIN object_registry oreg ON c.id = oreg.id AND oreg.erdate IS NULL "
                "JOIN object_state os ON os.object_id = oreg.id AND os.valid_to IS NULL "
                "JOIN enum_object_states eos ON eos.id = os.state_id "
                "WHERE eos.name = $1::text AND oreg.name = $2::text",
                Database::query_param_list
                    (Conversion::Enums::to_db_handle(Object_State::identified_contact))
                    (*i));

            if (contact_check.size() == 1) filtered.push_back(*i);
        }
        return filtered;
    }
};

class FilterIdentityAttached : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contacts) override
    {
        std::vector<std::string> filtered;
        std::for_each(contacts.begin(), contacts.end(), [&](auto&& contact_handle)
                {
                    const auto db_res = ctx.get_conn().exec_params(
                            "SELECT 0 "
                            "FROM object_registry obr "
                            "JOIN contact_identity ci ON ci.contact_id = obr.id "
                            "WHERE obr.erdate IS NULL AND "
                                  "obr.name = UPPER($1::TEXT) AND "
                                  "obr.type = get_object_type_id('contact') AND "
                                  "ci.valid_to IS NULL",
                            Database::query_param_list(contact_handle));
                    if (db_res.size() == 1)
                    {
                        filtered.push_back(contact_handle);
                    }
                });
        return filtered;
    }
};

class FilterConditionallyIdentifiedContact : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            const Database::Result contact_check = ctx.get_conn().exec_params(
                "SELECT oreg.name FROM contact c JOIN object_registry oreg ON c.id = oreg.id AND oreg.erdate IS NULL "
                "JOIN object_state os ON os.object_id = oreg.id AND os.valid_to IS NULL "
                "JOIN enum_object_states eos ON eos.id = os.state_id "
                "WHERE eos.name = $1::text AND oreg.name = $2::text",
                Database::query_param_list
                    (Conversion::Enums::to_db_handle(Object_State::conditionally_identified_contact))
                    (*i));

            if (contact_check.size() == 1)
            {
                filtered.push_back(*i);
            }
        }
        return filtered;
    }
};

class FilterHandleMojeIDSyntax : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext&,
            const std::vector<std::string>& contact_handle) override
    {
        boost::regex mojeid_handle_syntax("^[a-z0-9](-?[a-z0-9])*$");
        std::vector<std::string> filtered;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            if (boost::regex_match(boost::to_lower_copy(*i), mojeid_handle_syntax)
                    && (*i).length() <= 30)
            {
                filtered.push_back(*i);
            }
        }
        return filtered;
    }
};

class FilterMaxDomainsBound : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;

        if (contact_handle.empty()) return filtered;

        std::string query_begin("SELECT cc.handle, (cc.domain_registrant_count + cc.domain_admin_count) AS all_domains_count FROM ( "
        " SELECT current_contact.handle "
        " , (SELECT count(*) FROM object_registry oreg JOIN domain d ON oreg.id = d.id WHERE d.registrant = current_contact.id) AS domain_registrant_count "
        " , (SELECT count(*) FROM object_registry oreg JOIN domain d ON oreg.id = d.id JOIN domain_contact_map dcm ON dcm.domainid = d.id and dcm.role = 1 "
        "    WHERE dcm.contactid  = current_contact.id) AS domain_admin_count "
        " FROM (SELECT oreg.name AS handle, c.id AS id FROM contact c JOIN object_registry oreg ON c.id = oreg.id AND oreg.erdate IS NULL");

        Util::HeadSeparator where_or(" WHERE ", " OR ");

        std::string query_end(") AS current_contact"
        " ) cc "
        " ORDER BY all_domains_count DESC ");

        Database::QueryParams params;//query params
        std::ostringstream sql;
        sql << query_begin;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            params.push_back(*i);
            sql << where_or.get() << "oreg.name = UPPER($" << params.size() << "::text) ";
        }
        sql << query_end;

        const Database::Result contact_domains = ctx.get_conn().exec_params(sql.str(), params);

        for (Database::Result::size_type i = 0 ; i < contact_domains.size(); ++i)
        {
            //if it is first contact with maximum of domains bound or another contact with the same maximum number of domains bound
            if ((i == 0) || (std::string(contact_domains[0][1]).compare(std::string(contact_domains[i][1])) == 0 ))
            {
                filtered.push_back(std::string(contact_domains[i][0]));
            }
            else
            {//ignore others
                break;
            }
        }
        return filtered;
    }
};

class FilterMaxObjectsBound : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;

        if (contact_handle.empty()) return filtered;


        std::string query_begin("SELECT cc.handle, (cc.domain_registrant_count + cc.domain_admin_count + cc.nsset_tech_count + cc.keyset_tech_count) AS all_domains_count FROM ( "
        " SELECT current_contact.handle "
        " , (SELECT count(*) FROM object_registry oreg JOIN domain d ON oreg.id = d.id WHERE d.registrant = current_contact.id) AS domain_registrant_count "
        " , (SELECT count(*) FROM object_registry oreg JOIN domain d ON oreg.id = d.id JOIN domain_contact_map dcm ON dcm.domainid = d.id and dcm.role = 1 "
        "    WHERE dcm.contactid  = current_contact.id) AS domain_admin_count "
        " , (SELECT count(*) FROM object_registry oreg JOIN nsset n ON oreg.id = n.id JOIN nsset_contact_map ncm ON ncm.nssetid = n.id "
        "    WHERE ncm.contactid  = current_contact.id) AS nsset_tech_count "
        " , (SELECT count(*) FROM object_registry oreg JOIN keyset k ON oreg.id = k.id JOIN keyset_contact_map kcm ON kcm.keysetid = k.id "
        "    WHERE kcm.contactid  = current_contact.id) AS keyset_tech_count "
        " FROM (SELECT oreg.name AS handle, c.id AS id FROM contact c JOIN object_registry oreg ON c.id = oreg.id AND oreg.erdate IS NULL");

        Util::HeadSeparator where_or(" WHERE ", " OR ");

        std::string query_end(") AS current_contact"
        " ) cc "
        " ORDER BY all_domains_count DESC ");

        Database::QueryParams params;//query params
        std::ostringstream sql;
        sql << query_begin;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            params.push_back(*i);
            sql << where_or.get() << "oreg.name = UPPER($" << params.size() << "::text) ";
        }
        sql << query_end;

        const Database::Result contact_objects = ctx.get_conn().exec_params(sql.str(), params);

        for (Database::Result::size_type i = 0 ; i < contact_objects.size(); ++i)
        {
            //if it is first contact with maximum of objects bound or another contact with the same maximum number of objects bound
            if ((i == 0) || (std::string(contact_objects[0][1]).compare(std::string(contact_objects[i][1])) == 0 ))
            {
                filtered.push_back(std::string(contact_objects[i][0]));
            }
            else
            {//ignore others
                break;
            }
        }

        return filtered;
    }
};

class FilterRecentlyUpdated : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;

        if (contact_handle.empty()) return filtered;

        std::string query_begin("SELECT oreg.name, o.update FROM object o "
                " JOIN object_registry oreg ON o.id = oreg.id AND o.update IS NOT NULL AND oreg.erdate IS NULL"
                " JOIN contact c ON c.id = oreg.id ");

        Util::HeadSeparator where_or(" WHERE ", " OR ");

        std::string query_end(" ORDER BY o.update DESC ");

        Database::QueryParams params;//query params
        std::ostringstream sql;
        sql << query_begin;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            params.push_back(*i);
            sql << where_or.get() << "oreg.name = UPPER($" << params.size() << "::text) ";
        }
        sql << query_end;

        const Database::Result contact_updated = ctx.get_conn().exec_params(sql.str(), params);

        for (Database::Result::size_type i = 0 ; i < contact_updated.size(); ++i)
        {
            //if it is first contact with most recent update timestamp or another contact with the same update timestamp
            if ((i == 0) || (std::string(contact_updated[0][1]).compare(std::string(contact_updated[i][1])) == 0 ))
            {
                filtered.push_back(std::string(contact_updated[i][0]));
            }
            else
            {//ignore others
                break;
            }
        }

        return filtered;
    }
};

class FilterNotRegCzNic : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;

        std::string query(
            "SELECT oreg.name FROM object_registry oreg "
            " JOIN object o ON oreg.id = o.id AND oreg.erdate IS NULL"
            " JOIN registrar r ON r.id = o.clid AND r.handle != 'REG-CZNIC' "
            " JOIN contact c ON c.id = oreg.id ");

        Util::HeadSeparator where_or(" WHERE ", " OR ");

        Database::QueryParams params;//query params
        std::ostringstream sql;
        sql << query;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            params.push_back(*i);
            sql << where_or.get() << "oreg.name = UPPER($" << params.size() << "::text) ";
        }

        const Database::Result contact_not_regcznic = ctx.get_conn().exec_params(sql.str(), params);

        for (Database::Result::size_type i = 0 ; i < contact_not_regcznic.size(); ++i)
        {
                filtered.push_back(std::string(contact_not_regcznic[i][0]));
        }

        return filtered;
    }
};

class FilterRecentlyCreated : public ContactSelectionFilter
{
private:
    std::vector<std::string> operator()(
            const OperationContext& ctx,
            const std::vector<std::string>& contact_handle) override
    {
        std::vector<std::string> filtered;

        if (contact_handle.empty()) return filtered;

        std::string query_begin("SELECT oreg.name "
                " FROM object_registry oreg "
                " JOIN contact c ON c.id = oreg.id AND oreg.erdate IS NULL");

        Util::HeadSeparator where_or(" WHERE ", " OR ");

        std::string query_end(" ORDER BY oreg.crdate DESC, oreg.id DESC LIMIT 1 ");

        Database::QueryParams params;//query params
        std::ostringstream sql;
        sql << query_begin;
        for (std::vector<std::string>::const_iterator i = contact_handle.begin(); i != contact_handle.end() ; ++i)
        {
            params.push_back(*i);
            sql << where_or.get() << "oreg.name = UPPER($" << params.size() << "::text) ";
        }
        sql << query_end;

        const Database::Result contact_created = ctx.get_conn().exec_params(sql.str(), params);

        if (contact_created.size() == 1)
        {
            filtered.push_back(std::string(contact_created[0][0]));
        }

        return filtered;
    }
};

}//namespace LibFred::{anonymous}

}//namespace LibFred

const LibFred::ContactSelectionFilterFactory& LibFred::get_default_contact_selection_filter_factory()
{
    static thread_local const auto factory = []()
    {
        ContactSelectionFilterFactory factory{};
        factory.add_producer({MCS_FILTER_IDENTIFIED_CONTACT, std::make_unique<FilterIdentifiedContact>()})
               .add_producer({MCS_FILTER_IDENTITY_ATTACHED, std::make_unique<FilterIdentityAttached>()})
               .add_producer({MCS_FILTER_CONDITIONALLY_IDENTIFIED_CONTACT, std::make_unique<FilterConditionallyIdentifiedContact>()})
               .add_producer({MCS_FILTER_HANDLE_MOJEID_SYNTAX, std::make_unique<FilterHandleMojeIDSyntax>()})
               .add_producer({MCS_FILTER_MAX_DOMAINS_BOUND, std::make_unique<FilterMaxDomainsBound>()})
               .add_producer({MCS_FILTER_MAX_OBJECTS_BOUND, std::make_unique<FilterMaxObjectsBound>()})
               .add_producer({MCS_FILTER_RECENTLY_UPDATED, std::make_unique<FilterRecentlyUpdated>()})
               .add_producer({MCS_FILTER_NOT_REGCZNIC, std::make_unique<FilterNotRegCzNic>()})
               .add_producer({MCS_FILTER_RECENTLY_CREATED, std::make_unique<FilterRecentlyCreated>()});
        return factory;
    }();
    return factory;
}
