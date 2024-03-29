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

#include "libfred/registrable_object/nsset/create_nsset.hh"
#include "libfred/registrable_object/nsset/copy_history_impl.hh"
#include "libfred/object/object.hh"
#include "libfred/object/object_impl.hh"
#include "libfred/registrar/registrar_impl.hh"
#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/util.hh"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace LibFred {

CreateNsset::CreateNsset(const std::string& handle
            , const std::string& registrar)
: handle_(handle)
, registrar_(registrar)
{}

CreateNsset::CreateNsset(const std::string& handle
        , const std::string& registrar
        , const Optional<std::string>&
        , const Optional<short>& tech_check_level
        , const std::vector<DnsHost>& dns_hosts
        , const std::vector<std::string>& tech_contacts
        , const Optional<unsigned long long>& logd_request_id
        )
: handle_(handle)
, registrar_(registrar)
, tech_check_level_(tech_check_level)
, dns_hosts_(dns_hosts)
, tech_contacts_(tech_contacts)
, logd_request_id_(logd_request_id.isset()
        ? Nullable<unsigned long long>(logd_request_id.get_value())
        : Nullable<unsigned long long>())//is NULL if not set
{}

CreateNsset& CreateNsset::set_authinfo(const std::string&)
{
    return *this;
}

CreateNsset& CreateNsset::set_tech_check_level(short tech_check_level)
{
    tech_check_level_ = tech_check_level;
    return *this;
}

CreateNsset& CreateNsset::set_dns_hosts(const std::vector<DnsHost>& dns_hosts)
{
    dns_hosts_ = dns_hosts;
    return *this;
}

CreateNsset& CreateNsset::set_tech_contacts(const std::vector<std::string>& tech_contacts)
{
    tech_contacts_ = tech_contacts;
    return *this;
}

CreateNsset& CreateNsset::set_logd_request_id(unsigned long long logd_request_id)
{
    logd_request_id_ = logd_request_id;
    return *this;
}

CreateNsset::Result CreateNsset::exec(const OperationContext& ctx, const std::string& returned_timestamp_pg_time_zone_name)
{
    Result result;

    try
    {
        Exception create_nsset_exception;
        try
        {
            result.create_object_result = CreateObject(
                "nsset", handle_, registrar_, logd_request_id_)
                .exec(ctx);
        }
        catch (const CreateObject::Exception& create_object_exception)
        {
            //CreateObject implementation sets only one member at once into Exception instance
            if (create_object_exception.is_set_unknown_registrar_handle())
            {
                //fatal good path, need valid registrar performing create
                BOOST_THROW_EXCEPTION(Exception().set_unknown_registrar_handle(
                        create_object_exception.get_unknown_registrar_handle()));
            }
            else if(create_object_exception.is_set_invalid_object_handle())
            {   //non-fatal good path, create can continue to check input
                create_nsset_exception.set_invalid_nsset_handle(
                        create_object_exception.get_invalid_object_handle());
            }
            else
            {
                throw;//rethrow unexpected
            }
        }

        //create nsset
        {
            Database::QueryParams params;//query params
            std::ostringstream col_sql;
            std::ostringstream val_sql;
            Util::HeadSeparator col_separator("", ",");
            Util::HeadSeparator val_separator("", ",");

            col_sql <<"INSERT INTO nsset (";
            val_sql << " VALUES (";

            //id
            params.push_back(result.create_object_result.object_id);
            col_sql << col_separator.get() << "id";
            val_sql << val_separator.get() << "$" << params.size() <<"::integer";

            if (tech_check_level_.isset())
            {
                params.push_back(tech_check_level_.get_value());
                col_sql << col_separator.get() << "checklevel";
                val_sql << val_separator.get() << "$" << params.size() <<"::smallint";
            }

            col_sql <<")";
            val_sql << ")";
            //insert
            ctx.get_conn().exec_params(col_sql.str() + val_sql.str(), params);

            //set dns hosts
            if (!dns_hosts_.empty())
            {
                for (std::vector<DnsHost>::iterator i = dns_hosts_.begin(); i != dns_hosts_.end(); ++i)
                {
                    unsigned long long add_host_id = 0;
                    try
                    {
                        ctx.get_conn().exec("SAVEPOINT dnshost");
                        const Database::Result add_host_id_res = ctx.get_conn().exec_params(
                        "INSERT INTO host (nssetid, fqdn) VALUES( "
                        " $1::integer, LOWER($2::text)) RETURNING id"
                        , Database::query_param_list(result.create_object_result.object_id)(i->get_fqdn()));
                        ctx.get_conn().exec("RELEASE SAVEPOINT dnshost");

                        add_host_id = static_cast<unsigned long long>(add_host_id_res[0][0]);
                    }
                    catch (const std::exception& ex)
                    {
                        std::string what_string(ex.what());
                        if (what_string.find("host_nssetid_fqdn_key") != std::string::npos)
                        {
                            create_nsset_exception.add_already_set_dns_host(i->get_fqdn());
                            ctx.get_conn().exec("ROLLBACK TO SAVEPOINT dnshost");
                        }
                        else {
                            throw;
                        }
                    }

                    std::vector<boost::asio::ip::address> dns_host_ip = i->get_inet_addr();

                    for (std::vector<boost::asio::ip::address>::iterator j = dns_host_ip.begin(); j != dns_host_ip.end(); ++j)
                    {
                        try
                        {
                            ctx.get_conn().exec("SAVEPOINT dnshostipaddr");
                            ctx.get_conn().exec_params(
                            "INSERT INTO host_ipaddr_map (hostid, nssetid, ipaddr) "
                            " VALUES($1::integer, $2::integer, $3::inet)"
                            , Database::query_param_list(add_host_id)(result.create_object_result.object_id)(*j));
                            ctx.get_conn().exec("RELEASE SAVEPOINT dnshostipaddr");
                        }
                        catch (const std::exception& ex)
                        {
                            std::string what_string(ex.what());
                            if (what_string.find("syntax for type inet") != std::string::npos)
                            {
                                create_nsset_exception.add_invalid_dns_host_ipaddr(j->to_string());
                                ctx.get_conn().exec("ROLLBACK TO SAVEPOINT dnshostipaddr");
                            }
                            else {
                                throw;
                            }
                        }
                    }
                }
            }

            //set tech contacts
            if (!tech_contacts_.empty())
            {
                Database::QueryParams params;//query params
                std::ostringstream sql;

                params.push_back(result.create_object_result.object_id);
                sql << "INSERT INTO nsset_contact_map(nssetid, contactid) "
                        " VALUES ($" << params.size() << "::integer, ";

                for (std::vector<std::string>::iterator i = tech_contacts_.begin(); i != tech_contacts_.end(); ++i)
                {
                    //lock object_registry row for share and get id
                    const unsigned long long tech_contact_id = get_object_id_by_handle_and_type_with_lock(
                            ctx, false, *i, "contact", &create_nsset_exception,
                            &Exception::add_unknown_technical_contact_handle);
                    if (tech_contact_id == 0) continue;

                    Database::QueryParams params_i = params;//query params
                    std::ostringstream sql_i;
                    sql_i << sql.str();

                    params_i.push_back(tech_contact_id);
                    sql_i << " $"<< params_i.size() << "::integer )";

                    try
                    {
                        ctx.get_conn().exec("SAVEPOINT tech_contact");
                        ctx.get_conn().exec_params(sql_i.str(), params_i);
                        ctx.get_conn().exec("RELEASE SAVEPOINT tech_contact");
                    }
                    catch (const std::exception& ex)
                    {
                        std::string what_string(ex.what());
                        if (what_string.find("nsset_contact_map_pkey") != std::string::npos)
                        {
                            create_nsset_exception.add_already_set_technical_contact_handle(*i);
                            ctx.get_conn().exec("ROLLBACK TO SAVEPOINT tech_contact");
                        }
                        else {
                            throw;
                        }
                    }
                }
            }

            //check exception
            if (create_nsset_exception.throw_me())
                BOOST_THROW_EXCEPTION(create_nsset_exception);

            //get crdate from object_registry
            {
                const Database::Result crdate_res = ctx.get_conn().exec_params(
                        "SELECT crdate::timestamp AT TIME ZONE 'UTC' AT TIME ZONE $1::text "
                        "  FROM object_registry "
                        " WHERE id = $2::bigint"
                    , Database::query_param_list(returned_timestamp_pg_time_zone_name)(result.create_object_result.object_id));
                if (crdate_res.size() != 1)
                {
                    BOOST_THROW_EXCEPTION(LibFred::InternalError("timestamp of the nsset creation was not found"));
                }
                result.creation_time = boost::posix_time::time_from_string(std::string(crdate_res[0][0]));
            }
        }

        copy_nsset_data_to_nsset_history_impl(ctx, result.create_object_result.object_id, result.create_object_result.history_id);

    }
    catch (ExceptionStack& ex)
    {
        ex.add_exception_stack_info(to_string());
        throw;
    }

    return result;
}

std::string CreateNsset::to_string() const
{
    return Util::format_operation_state(
            "CreateNsset",
            Util::vector_of<std::pair<std::string, std::string>>
                (std::make_pair("handle", handle_))
                (std::make_pair("registrar", registrar_))
                (std::make_pair("dns_hosts", Util::format_container(dns_hosts_)))
                (std::make_pair("tech_contacts", Util::format_container(tech_contacts_)))
                (std::make_pair("logd_request_id", logd_request_id_.print_quoted())));
}

const std::string& CreateNsset::get_handle()const
{
    return handle_;
}

}//namespace LibFred
