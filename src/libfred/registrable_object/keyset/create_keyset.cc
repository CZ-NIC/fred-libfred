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

#include "libfred/registrable_object/keyset/create_keyset.hh"
#include "libfred/registrable_object/keyset/copy_history_impl.hh"
#include "libfred/object/object.hh"
#include "libfred/object/object_impl.hh"
#include "libfred/object/object_type.hh"
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

CreateKeyset::CreateKeyset(const std::string& handle, const std::string& registrar)
    : handle_(handle),
      registrar_(registrar)
{}

CreateKeyset::CreateKeyset(const std::string& handle
        , const std::string& registrar
        , const Optional<std::string>&
        , const std::vector<DnsKey>& dns_keys
        , const std::vector<std::string>& tech_contacts)
: handle_(handle)
, registrar_(registrar)
, dns_keys_(dns_keys)
, tech_contacts_(tech_contacts)
{}

CreateKeyset& CreateKeyset::set_authinfo(const std::string&)
{
    return *this;
}

CreateKeyset& CreateKeyset::set_dns_keys(const std::vector<DnsKey>& dns_keys)
{
    dns_keys_ = dns_keys;
    return *this;
}

CreateKeyset& CreateKeyset::set_tech_contacts(const std::vector<std::string>& tech_contacts)
{
    tech_contacts_ = tech_contacts;
    return *this;
}

CreateKeyset::Result CreateKeyset::exec(const OperationContext& _ctx,
                                        const Optional<unsigned long long>& _logd_request_id,
                                        const std::string& _returned_timestamp_pg_time_zone_name)
{
    try
    {
        Result result;
        //check registrar
        Registrar::get_registrar_id_by_handle(
            _ctx, registrar_, static_cast<Exception*>(0)//set throw
            , &Exception::set_unknown_registrar_handle);

        result.create_object_result =
            CreateObject(Conversion::Enums::to_db_handle(Object_Type::keyset),
                         handle_,
                         registrar_,
                         _logd_request_id.isset() ? _logd_request_id.get_value()
                                                  : Nullable< unsigned long long >()).exec(_ctx);

        Exception create_keyset_exception;

        //create keyset
        {
            //insert
            _ctx.get_conn().exec_params("INSERT INTO keyset (id) VALUES ($1::integer)"
                    , Database::query_param_list(result.create_object_result.object_id));

            //set dns keys
            if (!dns_keys_.empty())
            {
                for (std::vector<DnsKey>::const_iterator i = dns_keys_.begin(); i != dns_keys_.end(); ++i)
                {
                    try
                    {
                        _ctx.get_conn().exec("SAVEPOINT dnskey");
                        _ctx.get_conn().exec_params(
                        "INSERT INTO dnskey (keysetid, flags, protocol, alg, key) VALUES($1::integer "
                        ", $2::integer, $3::integer, $4::integer, $5::text)"
                        , Database::query_param_list(result.create_object_result.object_id)
                                                    (i->get_flags())
                                                    (i->get_protocol())
                                                    (i->get_alg())
                                                    (i->get_key()));
                        _ctx.get_conn().exec("RELEASE SAVEPOINT dnskey");
                    }
                    catch (const std::exception& ex)
                    {
                        std::string what_string(ex.what());
                        if (what_string.find("dnskey_unique_key") != std::string::npos)
                        {
                            create_keyset_exception.add_already_set_dns_key(*i);
                            _ctx.get_conn().exec("ROLLBACK TO SAVEPOINT dnskey");
                        }
                        else {
                            throw;
                        }
                    }
                }//for i
            }//if set dns keys


            //set tech contacts
            if (!tech_contacts_.empty())
            {
                Database::QueryParams params;//query params
                std::ostringstream sql;

                params.push_back(result.create_object_result.object_id);
                sql << "INSERT INTO keyset_contact_map(keysetid, contactid) "
                        " VALUES ($" << params.size() << "::integer, ";

                for (std::vector<std::string>::const_iterator i = tech_contacts_.begin(); i != tech_contacts_.end(); ++i)
                {
                    //lock object_registry row for share and get id
                    unsigned long long tech_contact_id = get_object_id_by_handle_and_type_with_lock(
                            _ctx, false, *i,
                            Conversion::Enums::to_db_handle(Object_Type::contact),
                            &create_keyset_exception,
                            &Exception::add_unknown_technical_contact_handle);
                    if (tech_contact_id == 0) continue;

                    Database::QueryParams params_i = params;//query params
                    std::ostringstream sql_i;
                    sql_i << sql.str();

                    params_i.push_back(tech_contact_id);
                    sql_i << " $" << params_i.size() << "::integer) ";

                    try
                    {
                        _ctx.get_conn().exec("SAVEPOINT tech_contact");
                        _ctx.get_conn().exec_params(sql_i.str(), params_i);
                        _ctx.get_conn().exec("RELEASE SAVEPOINT tech_contact");
                    }
                    catch (const std::exception& ex)
                    {
                        std::string what_string(ex.what());
                        if (what_string.find("keyset_contact_map_pkey") != std::string::npos)
                        {
                            create_keyset_exception.add_already_set_technical_contact_handle(*i);
                            _ctx.get_conn().exec("ROLLBACK TO SAVEPOINT tech_contact");
                        }
                        else {
                            throw;
                        }
                    }
                }//for i
            }//if set tech contacts


            //get crdate from object_registry
            {
                const Database::Result crdate_res = _ctx.get_conn().exec_params(
                        "SELECT crdate::timestamp AT TIME ZONE 'UTC' AT TIME ZONE $1::text "
                        "  FROM object_registry "
                        " WHERE id = $2::bigint"
                    , Database::query_param_list(_returned_timestamp_pg_time_zone_name)
                                                (result.create_object_result.object_id));

                if (crdate_res.size() != 1)
                {
                    BOOST_THROW_EXCEPTION(LibFred::InternalError("timestamp of the keyset creation was not found"));
                }

                result.creation_time = boost::posix_time::time_from_string(std::string(crdate_res[0][0]));
            }
        }

        //check exception
        if (create_keyset_exception.throw_me()) {
            BOOST_THROW_EXCEPTION(create_keyset_exception);
        }

        copy_keyset_data_to_keyset_history_impl(_ctx,
                                                result.create_object_result.object_id,
                                                result.create_object_result.history_id);
        return result;
    }
    catch (ExceptionStack& ex)
    {
        ex.add_exception_stack_info(this->to_string());
        throw;
    }
}

std::string CreateKeyset::to_string()const
{
    return Util::format_operation_state(
            "CreateKeyset",
            Util::vector_of<std::pair<std::string, std::string>>
                (std::make_pair("handle", handle_))
                (std::make_pair("registrar", registrar_))
                (std::make_pair("dns_keys", Util::format_container(dns_keys_)))
                (std::make_pair("tech_contacts", Util::format_container(tech_contacts_))));
}

const std::string& CreateKeyset::get_handle()const
{
    return handle_;
}

}//namespace LibFred
