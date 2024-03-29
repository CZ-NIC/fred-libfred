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

#include "libfred/registrable_object/nsset/info_nsset_impl.hh"

#include "libfred/db_settings.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrable_object/contact/contact_reference.hh"
#include "libfred/registrable_object/contact/contact_uuid.hh"

#include "util/db/nullable.hh"
#include "util/db/param_query_composition.hh"
#include "util/util.hh"

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_period.hpp>

#include <string>
#include <vector>
#include <utility>
#include <sstream>

namespace LibFred {

InfoNsset::InfoNsset()
    : history_query_(false),
      lock_(false)
{}

InfoNsset& InfoNsset::set_history_query(bool history_query)
{
    history_query_ = history_query;
    return *this;
}

InfoNsset& InfoNsset::set_lock()
{
    lock_ = true;
    return *this;
}

InfoNsset& InfoNsset::set_inline_view_filter(const Database::ParamQuery& filter_expr)
{
    info_nsset_inline_view_filter_expr_ = filter_expr;
    return *this;
}

InfoNsset& InfoNsset::set_cte_id_filter(const Database::ParamQuery& cte_id_filter_query)
{
    info_nsset_id_filter_cte_ = cte_id_filter_query;
    return *this;
}

Database::ParamQuery InfoNsset::make_info_nsset_projection_query(const std::string& local_timestamp_pg_time_zone_name)const
{
    const Database::ReusableParameter p_local_zone(local_timestamp_pg_time_zone_name, "text");
    Database::ParamQuery info_nsset_query;

    if (info_nsset_id_filter_cte_.isset())
    {
        info_nsset_query("WITH id_filter(id) AS (")(info_nsset_id_filter_cte_.get_value())(") ");
    }

    info_nsset_query(
            "SELECT * FROM ("
            "SELECT nobr.id AS ")(GetAlias::id())(","
                   "nobr.uuid AS ")(GetAlias::uuid())(","
                   "nobr.roid AS ")(GetAlias::roid())(","
                   "nobr.name AS ")(GetAlias::handle())(","
                   "(nobr.erdate AT TIME ZONE 'UTC' ) AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::delete_time())(","
                   "h.id AS ")(GetAlias::historyid())(","
                   "h.uuid AS ")(GetAlias::history_uuid())(","
                   "h.next AS ")(GetAlias::next_historyid())(","
                   "(h.valid_from AT TIME ZONE 'UTC') AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::history_valid_from())(","
                   "(h.valid_to AT TIME ZONE 'UTC') AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::history_valid_to())(","
                   "obj.clid AS ")(GetAlias::sponsoring_registrar_id())(","
                   "clr.handle AS ")(GetAlias::sponsoring_registrar_handle())(","
                   "nobr.crid AS ")(GetAlias::creating_registrar_id())(","
                   "crr.handle AS ")(GetAlias::creating_registrar_handle())(","
                   "obj.upid AS ")(GetAlias::last_updated_by_registrar_id())(","
                   "upr.handle AS ")(GetAlias::last_updated_by_registrar_handle())(","
                   "(nobr.crdate AT TIME ZONE 'UTC') AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::creation_time())(","
                   "(obj.trdate AT TIME ZONE 'UTC') AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::transfer_time())(","
                   "(obj.update AT TIME ZONE 'UTC') AT TIME ZONE ").param(p_local_zone)(" AS ")(GetAlias::update_time())(","
                   "nt.checklevel AS ")(GetAlias::tech_check_level())(","
                   "nobr.crhistoryid AS ")(GetAlias::first_historyid())(","
                   "h.request_id AS ")(GetAlias::logd_request_id())(","
                   "(CURRENT_TIMESTAMP AT TIME ZONE 'UTC')::timestamp AS ")(GetAlias::utc_timestamp())(","
                   /* CURRENT_TIMESTAMP is of type TIMESTAMP WITH TIME ZONE Ticket #15178 */
                   "(CURRENT_TIMESTAMP AT TIME ZONE ").param(p_local_zone)(")::timestamp AS ")(GetAlias::local_timestamp())(" "
            "FROM object_registry nobr ");

    if (history_query_)
    {
        info_nsset_query(
                "JOIN object_history obj ON obj.id=nobr.id "
                "JOIN nsset_history nt ON nt.historyid=obj.historyid "
                "JOIN history h ON h.id=nt.historyid ");
    }
    else
    {
        info_nsset_query(
                "JOIN object obj ON obj.id=nobr.id "
                "JOIN nsset nt ON nt.id=obj.id "
                "JOIN history h ON h.id=nobr.historyid ");
    }
    info_nsset_query(
            "JOIN registrar clr ON clr.id=obj.clid "
            "JOIN registrar crr ON crr.id=nobr.crid "
            "LEFT JOIN registrar upr ON upr.id=obj.upid "
            "WHERE nobr.type=get_object_type_id('nsset')");

    if (info_nsset_id_filter_cte_.isset())
    {
        info_nsset_query(" AND nobr.id IN (SELECT id FROM id_filter)");
    }

    if (!history_query_)
    {
        info_nsset_query(" AND nobr.erdate IS NULL");
    }

    if (lock_)
    {
        info_nsset_query(" FOR UPDATE OF nobr");
    }
    else
    {
        info_nsset_query(" FOR SHARE OF nobr");
    }
    info_nsset_query(") AS tmp");

    //inline view sub-select locking example at:
    //http://www.postgresql.org/docs/9.1/static/sql-select.html#SQL-FOR-UPDATE-SHARE
    if (info_nsset_inline_view_filter_expr_.isset())
    {
        info_nsset_query(" WHERE ")(info_nsset_inline_view_filter_expr_.get_value());
    }

    info_nsset_query(" ORDER BY ")(GetAlias::historyid())(" DESC");

    return info_nsset_query;
}

Database::ParamQuery InfoNsset::make_tech_contact_query(
        unsigned long long id,
        unsigned long long historyid)const
{
    Database::ParamQuery technical_contacts;

    technical_contacts("SELECT cobr.id AS tech_contact_id,cobr.name AS tech_contact_handle,cobr.uuid AS tech_contact_uuid ");
    if (history_query_)
    {
        technical_contacts(
                "FROM nsset_contact_map_history ncm "
                "JOIN object_registry cobr ON cobr.id=ncm.contactid "
                "WHERE ncm.nssetid=").param_bigint(id)(" AND "
                      "ncm.historyid=").param_bigint(historyid)(" AND "
                      "cobr.type=get_object_type_id('contact')");
    }
    else
    {
        technical_contacts(
                "FROM nsset_contact_map ncm "
                "JOIN object_registry cobr ON cobr.id=ncm.contactid AND cobr.erdate IS NULL "
                "WHERE ncm.nssetid = ").param_bigint(id)(" AND "
                      "cobr.type=get_object_type_id('contact')");
    }
    technical_contacts(" ORDER BY cobr.name");
    return technical_contacts;
}

Database::ParamQuery InfoNsset::make_dns_host_query(
        unsigned long long nssetid,
        unsigned long long historyid)const
{
    Database::ParamQuery query;

    query("SELECT h.nssetid AS host_nssetid,h.id AS host_id,h.fqdn AS host_fqdn ");
    if (history_query_)
    {
        query("FROM host_history h "
              "WHERE h.nssetid=").param_bigint(nssetid)(" AND "
                    "h.historyid=").param_bigint(historyid);
    }
    else
    {
        query("FROM host h "
              "JOIN object_registry nobr ON nobr.id=h.nssetid AND nobr.erdate IS NULL "
              "WHERE h.nssetid=").param_bigint(nssetid)(" AND "
                    "nobr.type=get_object_type_id('nsset')");
    }
    query(" ORDER BY h.fqdn");
    return query;
}

Database::ParamQuery InfoNsset::make_dns_ip_query(unsigned long long hostid, unsigned long long historyid)const
{
    Database::ParamQuery query;

    query("SELECT ipaddr AS host_ipaddr ");
    if (history_query_)
    {
        query("FROM host_ipaddr_map_history "
              "WHERE hostid=").param_bigint(hostid)(" AND "
                    "historyid=").param_bigint(historyid);
    }
    else
    {
        query("FROM host_ipaddr_map WHERE hostid=").param_bigint(hostid);
    }
    query(" ORDER BY ipaddr");
    return query;
}

std::vector<InfoNssetOutput> InfoNsset::exec(
        const OperationContext& ctx,
        const std::string& local_timestamp_pg_time_zone_name)const
{
    std::vector<InfoNssetOutput> result;

    const Database::Result param_query_result = ctx.get_conn().exec_params(
            make_info_nsset_projection_query(local_timestamp_pg_time_zone_name));

    result.reserve(param_query_result.size());

    for (Database::Result::size_type i = 0; i < param_query_result.size(); ++i)
    {
        InfoNssetOutput info_nsset_output;
        info_nsset_output.info_nsset_data.id = static_cast<unsigned long long>(param_query_result[i][GetAlias::id()]);
        info_nsset_output.info_nsset_data.uuid = param_query_result[i][GetAlias::uuid()].as<RegistrableObject::Nsset::NssetUuid>();
        info_nsset_output.info_nsset_data.roid = static_cast<std::string>(param_query_result[i][GetAlias::roid()]);
        info_nsset_output.info_nsset_data.handle = static_cast<std::string>(param_query_result[i][GetAlias::handle()]);
        info_nsset_output.info_nsset_data.delete_time = param_query_result[i][GetAlias::delete_time()].isnull() ? Nullable<boost::posix_time::ptime>()
            : Nullable<boost::posix_time::ptime>(boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::delete_time()])));
        info_nsset_output.info_nsset_data.historyid = static_cast<unsigned long long>(param_query_result[i][GetAlias::historyid()]);
        info_nsset_output.info_nsset_data.history_uuid = param_query_result[i][GetAlias::history_uuid()].as<RegistrableObject::Nsset::NssetHistoryUuid>();
        info_nsset_output.next_historyid = param_query_result[i][GetAlias::next_historyid()].isnull() ? Nullable<unsigned long long>()
            : Nullable<unsigned long long>(static_cast<unsigned long long>(param_query_result[i][GetAlias::next_historyid()]));
        info_nsset_output.history_valid_from = boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::history_valid_from()]));
        info_nsset_output.history_valid_to = param_query_result[i][GetAlias::history_valid_to()].isnull() ? Nullable<boost::posix_time::ptime>()
            : Nullable<boost::posix_time::ptime>(boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::history_valid_to()])));
        info_nsset_output.info_nsset_data.sponsoring_registrar_handle = static_cast<std::string>(param_query_result[i][GetAlias::sponsoring_registrar_handle()]);
        info_nsset_output.info_nsset_data.create_registrar_handle = static_cast<std::string>(param_query_result[i][GetAlias::creating_registrar_handle()]);
        info_nsset_output.info_nsset_data.update_registrar_handle = param_query_result[i][GetAlias::last_updated_by_registrar_handle()].isnull() ? Nullable<std::string>()
            : Nullable<std::string> (static_cast<std::string>(param_query_result[i][GetAlias::last_updated_by_registrar_handle()]));
        info_nsset_output.info_nsset_data.creation_time = boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::creation_time()]));
        info_nsset_output.info_nsset_data.transfer_time = param_query_result[i][GetAlias::transfer_time()].isnull() ? Nullable<boost::posix_time::ptime>()
            : Nullable<boost::posix_time::ptime>(boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::transfer_time()])));
        info_nsset_output.info_nsset_data.update_time = param_query_result[i][GetAlias::update_time()].isnull() ? Nullable<boost::posix_time::ptime>()
            : Nullable<boost::posix_time::ptime>(boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::update_time()])));
        info_nsset_output.info_nsset_data.tech_check_level = param_query_result[i][GetAlias::tech_check_level()].isnull() ? Nullable<short>()
            : Nullable<short>(static_cast<short>(param_query_result[i][GetAlias::tech_check_level()]));
        info_nsset_output.info_nsset_data.crhistoryid = static_cast<unsigned long long>(param_query_result[i][GetAlias::first_historyid()]);
        info_nsset_output.logd_request_id = param_query_result[i][GetAlias::logd_request_id()].isnull() ? Nullable<unsigned long long>()
            : Nullable<unsigned long long>(static_cast<unsigned long long>(param_query_result[i][GetAlias::logd_request_id()]));
        info_nsset_output.utc_timestamp = param_query_result[i][GetAlias::utc_timestamp()].isnull() ? boost::posix_time::ptime(boost::date_time::not_a_date_time)
            : boost::posix_time::time_from_string(static_cast<std::string>(param_query_result[i][GetAlias::utc_timestamp()]));

        //tech contacts
        const Database::Result tech_contact_res = ctx.get_conn().exec_params(make_tech_contact_query(
                info_nsset_output.info_nsset_data.id, info_nsset_output.info_nsset_data.historyid));
        info_nsset_output.info_nsset_data.tech_contacts.reserve(tech_contact_res.size());
        for (Database::Result::size_type j = 0; j < tech_contact_res.size(); ++j)
        {
            info_nsset_output.info_nsset_data.tech_contacts.push_back(RegistrableObject::Contact::ContactReference(
                    static_cast<unsigned long long>(tech_contact_res[j]["tech_contact_id"]),
                    static_cast<std::string>(tech_contact_res[j]["tech_contact_handle"]),
                    tech_contact_res[j]["tech_contact_uuid"].as<RegistrableObject::Contact::ContactUuid>()));
        }

        //DNS keys
        const Database::Result dns_hosts_res = ctx.get_conn().exec_params(make_dns_host_query(
                info_nsset_output.info_nsset_data.id, info_nsset_output.info_nsset_data.historyid));
        info_nsset_output.info_nsset_data.dns_hosts.reserve(dns_hosts_res.size());
        for (Database::Result::size_type j = 0; j < dns_hosts_res.size(); ++j)
        {
            unsigned long long dns_host_id = static_cast<unsigned long long>(dns_hosts_res[j]["host_id"]);
            std::string dns_host_fqdn = static_cast<std::string>(dns_hosts_res[j]["host_fqdn"]);

            const Database::Result dns_ip_res = ctx.get_conn().exec_params(make_dns_ip_query(
                    dns_host_id, info_nsset_output.info_nsset_data.historyid ));
            std::vector<boost::asio::ip::address> dns_ip;
            dns_ip.reserve(dns_ip_res.size());
            for (Database::Result::size_type k = 0; k < dns_ip_res.size(); ++k)
            {
                dns_ip.push_back(boost::asio::ip::address::from_string(static_cast<std::string>(dns_ip_res[k]["host_ipaddr"])));
            }
            info_nsset_output.info_nsset_data.dns_hosts.push_back(DnsHost(dns_host_fqdn, dns_ip));
        }
        result.push_back(info_nsset_output);
    }
    return result;
}

}//namespace LibFred
