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
 *  domain check
 */

#ifndef CHECK_DOMAIN_HH_94109AC573AD470D9113D5EF51567F45
#define CHECK_DOMAIN_HH_94109AC573AD470D9113D5EF51567F45

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/zone/zone.hh"
#include "util/printable.hh"

#include <string>

namespace LibFred {

/**
 * Checking of domain properties.
 */
class CheckDomain : public Util::Printable<CheckDomain>
{
public:
    /**
    * check domain constructor
    * @param fqdn a fully qualified domain name.
    */
    CheckDomain(const std::string& fqdn, bool is_system_registrar = false);

    /**
    * check domain name syntax (RFC1123), domain have zone (found in registry) and zone specific rules.
    * @param ctx an operation context with database and logging interface.
    * @return true if invalid, false if ok
    */
    bool is_invalid_syntax(const OperationContext& ctx) const;

    /**
    * check if domain have zone found in registry.
    * @param ctx an operation context with database and logging interface.
    * @return true if bad, false if ok
    */
    bool is_bad_zone(const OperationContext& ctx) const;

    /**
    * check number of domain name labels allowed by zone.
    * if zone is not found, check fails
    * @param ctx an operation context with database and logging interface.
    * @return true if bad, false if ok
    */
    bool is_bad_length(const OperationContext& ctx) const;

    /**
    * check if domain name is on blacklist.
    * @param ctx an operation context with database and logging interface.
    * @return true if blacklisted, false if ok
    */
    bool is_blacklisted(const OperationContext& ctx) const;

    /**
    * check if domain name is registered.
    * @param ctx an operation context with database and logging interface.
    * @param conflicting_fqdn_out an conflicting domain identifier reference used for output if true is returned.
    * @return true if registered, false if not
    */
    bool is_registered(const OperationContext& ctx, std::string& conflicting_fqdn_out) const;
    /**
    * check if domain name is registered.
    * @param ctx an operation context with database and logging interface.
    * @return true if registered, false if not
    */
    bool is_registered(const OperationContext& ctx) const;

    /**
    * check if domain name is available for registration.
    * @param ctx an operation context with database and logging interface.
    * @return true if available, false if not
    */
    bool is_available(const OperationContext& ctx) const;

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string fqdn_;/**< domain identifier */
    bool is_system_registrar_;
};

}//namespace LibFred

#endif//CHECK_DOMAIN_HH_94109AC573AD470D9113D5EF51567F45
