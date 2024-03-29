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
/**
 *  @file
 *  nsset update
 */

#include "libfred/registrable_object/nsset/update_nsset.hh"
#include "libfred/registrable_object/nsset/copy_history_impl.hh"
#include "libfred/object/object.hh"
#include "libfred/object/object_impl.hh"
#include "libfred/registrar/registrar_impl.hh"
#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/log/log.hh"

#include <sstream>
#include <string>
#include <vector>

namespace LibFred {

UpdateNsset::UpdateNsset(const std::string& handle, const std::string& registrar)
    : handle_(handle),
      registrar_(registrar)
{}

UpdateNsset::UpdateNsset(
        const std::string& handle,
        const std::string& registrar,
        const Optional<std::string>& authinfo,
        const std::vector<DnsHost>& add_dns,
        const std::vector<std::string>& rem_dns,
        const std::vector<std::string>& add_tech_contact,
        const std::vector<std::string>& rem_tech_contact,
        const Optional<short>& tech_check_level,
        const Optional<unsigned long long>& logd_request_id)
: handle_(handle)
, registrar_(registrar)
, authinfo_(authinfo)
, add_dns_(add_dns)
, rem_dns_(rem_dns)
, add_tech_contact_(add_tech_contact)
, rem_tech_contact_(rem_tech_contact)
, tech_check_level_(tech_check_level)
, logd_request_id_(logd_request_id.isset()
    ? Nullable<unsigned long long>(logd_request_id.get_value())
    : Nullable<unsigned long long>())//is NULL if not set
{}

UpdateNsset& UpdateNsset::set_authinfo(const std::string& authinfo)
{
    authinfo_ = authinfo;
    return *this;
}

UpdateNsset& UpdateNsset::add_dns(const DnsHost& dns)
{
    add_dns_.push_back(dns);
    return *this;
}

UpdateNsset& UpdateNsset::rem_dns(const std::string& fqdn)
{
    rem_dns_.push_back(fqdn);
    return *this;
}

UpdateNsset& UpdateNsset::add_tech_contact(const std::string& tech_contact)
{
    add_tech_contact_.push_back(tech_contact);
    return *this;
}

UpdateNsset& UpdateNsset::rem_tech_contact(const std::string& tech_contact)
{
    rem_tech_contact_.push_back(tech_contact);
    return *this;
}

UpdateNsset& UpdateNsset::set_logd_request_id(unsigned long long logd_request_id)
{
    logd_request_id_ = logd_request_id;
    return *this;
}

UpdateNsset& UpdateNsset::set_tech_check_level(short tech_check_level)
{
    tech_check_level_ = tech_check_level;
    return *this;
}

unsigned long long UpdateNsset::exec(const OperationContext& ctx)
{
    namespace ip = boost::asio::ip;

    try
    {
        //check registrar
        Registrar::get_registrar_id_by_handle(
            ctx, registrar_, static_cast<Exception*>(0)//set throw
            , &Exception::set_unknown_registrar_handle);

        //lock row and get nsset_id
        unsigned long long nsset_id = get_object_id_by_handle_and_type_with_lock(
                ctx, true, handle_, "nsset",static_cast<Exception*>(0),
                &Exception::set_unknown_nsset_handle);

        Exception update_nsset_exception;
        unsigned long long history_id = 0;

        try
        {
            //update object
            history_id = LibFred::UpdateObject(
                    handle_,
                    "nsset",
                    registrar_,
                    authinfo_,
                    logd_request_id_).exec(ctx);
        }
        catch (const LibFred::UpdateObject::Exception& ex)
        {
            bool caught_exception_has_been_handled = false;

            if (ex.is_set_unknown_object_handle() ) {
                update_nsset_exception.set_unknown_nsset_handle( ex.get_unknown_object_handle() );
                caught_exception_has_been_handled = true;
            }

            if (ex.is_set_unknown_registrar_handle() ) {
                update_nsset_exception.set_unknown_registrar_handle( ex.get_unknown_registrar_handle() );
                caught_exception_has_been_handled = true;
            }

            if (! caught_exception_has_been_handled ) {
                throw;
            }
        }
        //update nsset tech check level
        if (tech_check_level_.isset() && tech_check_level_.get_value() >= 0)
        {
            const Database::Result update_checklevel_res = ctx.get_conn().exec_params(
                "UPDATE nsset SET checklevel = $1::smallint "
                " WHERE id = $2::integer RETURNING id"
                , Database::query_param_list(tech_check_level_.get_value())(nsset_id));
            if (update_checklevel_res.size() != 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("failed to update checklevel"));
            }
        }

        //add tech contacts
        if (!add_tech_contact_.empty())
        {
            Database::QueryParams params;//query params
            std::ostringstream sql;

            params.push_back(nsset_id);
            sql << "INSERT INTO nsset_contact_map(nssetid, contactid) "
                    " VALUES ($" << params.size() << "::integer, ";

            for (std::vector<std::string>::iterator i = add_tech_contact_.begin(); i != add_tech_contact_.end(); ++i)
            {
                //lock object_registry row for share
                unsigned long long tech_contact_id = get_object_id_by_handle_and_type_with_lock(
                        ctx, false, *i, "contact",&update_nsset_exception,
                        &Exception::add_unknown_technical_contact_handle);
                if (tech_contact_id == 0) continue;

                Database::QueryParams params_i = params;//query params
                std::ostringstream sql_i;
                sql_i << sql.str();

                params_i.push_back(tech_contact_id);
                sql_i << " $" << params_i.size() << "::integer) ";

                try
                {
                    ctx.get_conn().exec("SAVEPOINT add_tech_contact");
                    ctx.get_conn().exec_params(sql_i.str(), params_i);
                    ctx.get_conn().exec("RELEASE SAVEPOINT add_tech_contact");
                }
                catch (const std::exception& ex)
                {
                    std::string what_string(ex.what());
                    if (what_string.find("nsset_contact_map_pkey") != std::string::npos)
                    {
                        update_nsset_exception.add_already_set_technical_contact_handle(*i);
                        ctx.get_conn().exec("ROLLBACK TO SAVEPOINT add_tech_contact");
                    }
                    else {
                        throw;
                    }
                }

            }
        }

        //delete tech contacts
        if (!rem_tech_contact_.empty())
        {
            Database::QueryParams params;
            std::ostringstream sql;

            params.push_back(nsset_id);
            sql << "DELETE FROM nsset_contact_map WHERE nssetid = $" << params.size() << "::integer AND ";

            for (std::vector<std::string>::iterator i = rem_tech_contact_.begin(); i != rem_tech_contact_.end(); ++i)
            {
                //lock object_registry row for share
                unsigned long long tech_contact_id = get_object_id_by_handle_and_type_with_lock(
                        ctx, false, *i, "contact",&update_nsset_exception,
                        &Exception::add_unknown_technical_contact_handle);
                if (tech_contact_id == 0) continue;

                Database::QueryParams params_i = params;//query params
                std::ostringstream sql_i;
                sql_i << sql.str();

                params_i.push_back(tech_contact_id);
                sql_i << "contactid = $" << params_i.size() << "::integer "
                        " RETURNING nssetid";

                const Database::Result nsset_del_res = ctx.get_conn().exec_params(sql_i.str(), params_i);
                if (nsset_del_res.size() == 0)
                {
                    update_nsset_exception.add_unassigned_technical_contact_handle(*i);
                    continue;//for rem_tech_contact_
                }
                if (nsset_del_res.size() != 1)
                {
                    BOOST_THROW_EXCEPTION(InternalError("failed to delete technical contact"));
                }
            }
        }

        //delete dns hosts - before adding new ones
        if (!rem_dns_.empty())
        {
            for (std::vector<std::string>::iterator i = rem_dns_.begin(); i != rem_dns_.end(); ++i)
            {
                const Database::Result rem_host_id_res = ctx.get_conn().exec_params(
                    "DELETE FROM host WHERE LOWER(fqdn)=LOWER($1::text) AND"
                    " nssetid = $2::integer RETURNING id "
                    , Database::query_param_list(*i)(nsset_id));

                if (rem_host_id_res.size() == 0)
                {
                    update_nsset_exception.add_unassigned_dns_host(*i);
                    continue;//for rem_dns_
                }
                if (rem_host_id_res.size() != 1)
                {
                    BOOST_THROW_EXCEPTION(InternalError("failed to delete DNS host"));
                }

                unsigned long long rem_host_id = rem_host_id_res[0][0];

                ctx.get_conn().exec_params("DELETE FROM host_ipaddr_map WHERE hostid = $1::integer"
                    , Database::query_param_list(rem_host_id));
            }
        }

        //add dns hosts
        if (!add_dns_.empty())
        {
            for (std::vector<DnsHost>::iterator i = add_dns_.begin(); i != add_dns_.end(); ++i)
            {
                unsigned long long add_host_id = 0;
                try
                {
                    ctx.get_conn().exec("SAVEPOINT add_dns_host");
                    const Database::Result add_host_id_res = ctx.get_conn().exec_params(
                        "INSERT INTO host (nssetid, fqdn) VALUES( "
                        " $1::integer, LOWER($2::text)) RETURNING id"
                        , Database::query_param_list(nsset_id)(i->get_fqdn()));
                    ctx.get_conn().exec("RELEASE SAVEPOINT add_dns_host");
                    add_host_id = static_cast<unsigned long long>(add_host_id_res[0][0]);
                }
                catch (const std::exception& ex)
                {
                    std::string what_string(ex.what());
                    if (what_string.find("host_nssetid_fqdn_key") != std::string::npos)
                    {
                        update_nsset_exception.add_already_set_dns_host(i->get_fqdn());
                        ctx.get_conn().exec("ROLLBACK TO SAVEPOINT add_dns_host");
                        continue;//for add_dns_
                    }
                    else {
                        throw;
                    }
                }

                std::vector<ip::address> dns_host_ip = i->get_inet_addr();

                for (std::vector<ip::address>::iterator j = dns_host_ip.begin(); j != dns_host_ip.end(); ++j)
                {
                    try
                    {
                        ctx.get_conn().exec("SAVEPOINT add_dns_host_ipaddr");
                        ctx.get_conn().exec_params(
                        "INSERT INTO host_ipaddr_map (hostid, nssetid, ipaddr) "
                        " VALUES($1::integer, $2::integer, $3::inet)"
                        , Database::query_param_list(add_host_id)(nsset_id)(j->to_string()));
                        ctx.get_conn().exec("RELEASE SAVEPOINT add_dns_host_ipaddr");
                    }
                    catch (const std::exception& ex)
                    {
                        std::string what_string(ex.what());
                        if (what_string.find("syntax for type inet") != std::string::npos)
                        {
                            update_nsset_exception.add_invalid_dns_host_ipaddr(j->to_string());
                            ctx.get_conn().exec("ROLLBACK TO SAVEPOINT add_dns_host_ipaddr");
                        }
                        else {
                            throw;
                        }
                    }
                }
            }
        }

        //check exception
        if (update_nsset_exception.throw_me()) {
            BOOST_THROW_EXCEPTION(update_nsset_exception);
        }

        copy_nsset_data_to_nsset_history_impl(ctx, nsset_id, history_id);
        return history_id;
    }
    catch (ExceptionStack& ex)
    {
        ex.add_exception_stack_info(to_string());
        throw;
    }
}

std::string UpdateNsset::to_string() const
{
    return Util::format_operation_state(
            "UpdateNsset",
            Util::vector_of<std::pair<std::string, std::string>>
                (std::make_pair("handle", handle_))
                (std::make_pair("registrar", registrar_))
                (std::make_pair("authinfo", authinfo_.print_quoted()))
                (std::make_pair("add_tech_contact", Util::format_container(add_tech_contact_)))
                (std::make_pair("rem_tech_contact", Util::format_container(rem_tech_contact_)))
                (std::make_pair("add_dns_host", Util::format_container(add_dns_)))
                (std::make_pair("rem_dns_host", Util::format_container(rem_dns_)))
                (std::make_pair("tech_check_level", tech_check_level_.print_quoted()))
                (std::make_pair("logd_request_id", logd_request_id_.print_quoted())));
}

}//namespace LibFred
