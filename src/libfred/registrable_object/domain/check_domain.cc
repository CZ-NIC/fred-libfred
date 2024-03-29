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
 *  @file check_domain.cc
 *  domain check
 */

#include <string>

#include "libfred/registrable_object/domain/check_domain.hh"
#include "libfred/registrable_object/domain/domain_name.hh"
#include "libfred/zone/zone.hh"
#include "libfred/object/object.hh"

#include "libfred/opcontext.hh"
#include "libfred/db_settings.hh"

namespace LibFred
{
    CheckDomain::CheckDomain(const std::string& fqdn, const bool _is_system_registrar)
    : fqdn_(fqdn), is_system_registrar_(_is_system_registrar)
    {}

    bool CheckDomain::is_invalid_syntax(const OperationContext& ctx) const
    {
        try
        {
            if (!Domain::is_rfc1123_compliant_host_name(fqdn_)) {
                return true;
            }

            //remove optional root dot from fqdn
            std::string no_root_dot_fqdn = LibFred::Zone::rem_trailing_dot(fqdn_);

            //get zone
            Zone::Data zone;
            try
            {
                zone = Zone::find_zone_in_fqdn(ctx, no_root_dot_fqdn);
            }
            catch (const Zone::Exception& ex)
            {
                if (ex.is_set_unknown_zone_in_fqdn()
                        && (ex.get_unknown_zone_in_fqdn().compare(no_root_dot_fqdn) == 0))
                {
                    return true;//zone not found
                }
                else {
                    throw;
                }
            }

            //domain_name_validation
            if (!LibFred::Domain::DomainNameValidator(is_system_registrar_)
                .set_checker_names(LibFred::Domain::get_domain_name_validation_config_for_zone(ctx, zone.name))
                .set_zone_name(LibFred::Domain::DomainName(zone.name))
                .set_ctx(ctx)
                .exec(LibFred::Domain::DomainName(fqdn_), std::count(zone.name.begin(), zone.name.end(), '.') + 1) // skip zone labels
            )
            {
                return true;
            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return false;//meaning ok
    }

    bool CheckDomain::is_bad_zone(const OperationContext& ctx) const
    {
        try
        {
            //remove optional root dot from fqdn
            std::string no_root_dot_fqdn = LibFred::Zone::rem_trailing_dot(fqdn_);

            //zone
            Zone::Data zone;
            try
            {
                zone = Zone::find_zone_in_fqdn(ctx, no_root_dot_fqdn);
            }
            catch (const Zone::Exception& ex)
            {
                if (ex.is_set_unknown_zone_in_fqdn()
                        && (ex.get_unknown_zone_in_fqdn().compare(no_root_dot_fqdn) == 0))
                {
                    return true;
                }
                else {
                    throw;
                }
            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }

        return false;//meaning ok
    }

    bool CheckDomain::is_bad_length(const OperationContext& ctx) const
    {
        try
        {
            //remove optional root dot from fqdn
            std::string no_root_dot_fqdn = LibFred::Zone::rem_trailing_dot(fqdn_);

            //get zone
            Zone::Data zone;
            try
            {
                zone = Zone::find_zone_in_fqdn(ctx, no_root_dot_fqdn);
            }
            catch (const Zone::Exception& ex)
            {
                if (ex.is_set_unknown_zone_in_fqdn()
                        && (ex.get_unknown_zone_in_fqdn().compare(no_root_dot_fqdn) == 0))
                {
                    return true;//zone not found
                }
                else {
                    throw;
                }
            }

            //check number of labels
            if (std::count(no_root_dot_fqdn.begin(), no_root_dot_fqdn.end(), '.')+1//fqdn labels number
                > std::count(zone.name.begin(), zone.name.end(), '.')+1+zone.dots_max)//max labels by zone
            {
                return true;
            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return false;//meaning ok
    }

    bool CheckDomain::is_blacklisted(const OperationContext& ctx) const
    {
        try
        {
            //remove optional root dot from fqdn
            std::string no_root_dot_fqdn = LibFred::Zone::rem_trailing_dot(fqdn_);

            //check blacklist regexp for match with fqdn
            Database::Result bl_res  = ctx.get_conn().exec_params(
                "SELECT id FROM domain_blacklist b "
                "WHERE $1::text ~* b.regexp AND NOW()>=b.valid_from "
                "AND (b.valid_to ISNULL OR NOW()<b.valid_to) "
            , Database::query_param_list(no_root_dot_fqdn));
            if (bl_res.size() > 0)//positively blacklisted
            {
                return true;
            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return false;//meaning ok
    }

    bool CheckDomain::is_registered(const OperationContext& ctx, std::string& conflicting_fqdn_out) const
    {
        try
        {
            //remove optional root dot from fqdn
            std::string no_root_dot_fqdn = LibFred::Zone::rem_trailing_dot(fqdn_);

            //get zone
            Zone::Data zone;
            try
            {
                zone = Zone::find_zone_in_fqdn(ctx, no_root_dot_fqdn);
            }
            catch (const Zone::Exception& ex)
            {
                if (ex.is_set_unknown_zone_in_fqdn()
                        && (ex.get_unknown_zone_in_fqdn().compare(no_root_dot_fqdn) == 0))
                {
                    return false;//zone not found
                }
                else {
                    throw;
                }
            }

            if (zone.is_enum)
            {
                Database::Result conflicting_fqdn_res  = ctx.get_conn().exec_params(
                        "SELECT obr.name, obr.id "
                        "FROM object_registry obr "
                        "JOIN domain d ON d.id = obr.id " // helps to dramatically reduce the number of candidate objects!!!
                        "WHERE obr.type = get_object_type_id('domain'::TEXT) AND "
                              "obr.erdate IS NULL AND "
                              "(($1::TEXT LIKE ('%.' || obr.name)) OR "
                               "(obr.name LIKE ('%.' || $1::TEXT)) OR "
                               "(obr.name = LOWER($1::TEXT))) AND "
                              "d.zone = $2::INT "
                        "LIMIT 1",
                        Database::query_param_list(no_root_dot_fqdn)(zone.id));
                if (conflicting_fqdn_res.size() > 0)//have conflicting_fqdn
                {
                    conflicting_fqdn_out = static_cast<std::string>(conflicting_fqdn_res[0][0]);
                    return true;
                }
            }
            else
            {//is not ENUM
                Database::Result conflicting_fqdn_res  = ctx.get_conn().exec_params(
                    "SELECT o.name, o.id FROM object_registry o WHERE o.type=get_object_type_id('domain'::text) "
                    "AND o.erdate ISNULL AND o.name=LOWER($1::text) LIMIT 1"
                , Database::query_param_list(no_root_dot_fqdn));
                if (conflicting_fqdn_res.size() > 0)//have conflicting_fqdn
                {
                    conflicting_fqdn_out = static_cast<std::string>(conflicting_fqdn_res[0][0]);
                    return true;
                }

            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return false;//meaning ok
    }

    bool CheckDomain::is_registered(const OperationContext& ctx) const
    {
        std::string conflicting_fqdn_out;
        return is_registered(ctx, conflicting_fqdn_out);
    }


    bool CheckDomain::is_available(const OperationContext& ctx) const
    {
        try
        {
            if (is_invalid_syntax(ctx)
            || is_bad_length(ctx)
            || is_registered(ctx)
            || is_blacklisted(ctx))
            {
                return false;
            }
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return true;//meaning ok
    }

    std::string CheckDomain::to_string() const
    {
        return Util::format_operation_state("CheckDomain",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("fqdn", fqdn_))
        );
    }

} // namespace LibFred

