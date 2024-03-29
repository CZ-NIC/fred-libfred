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
 *  @file domain.h
 *  <++>
 */

#ifndef DOMAIN_HH_CD4EA3CAAAB3482BBE7CEA41B4E6FDCA
#define DOMAIN_HH_CD4EA3CAAAB3482BBE7CEA41B4E6FDCA

#include "libfred/opcontext.hh"

#include <exception>
#include <string>

namespace LibFred {
namespace Domain {

/**
* \brief Available domains need to be checked for zone-specific FQDN syntax validity
*        (i.e. must be DomainFqdnSyntaxValidity::valid too).
*/
struct DomainRegistrability
{
    enum Enum
    {
        registered,
        blacklisted,
        zone_not_in_registry,
        available,
    };
};

struct DomainFqdnSyntaxValidity
{
    enum Enum
    {
        valid,
        invalid,
    };
};

/**
 * \brief  Returns whether the domain's fully qualified name is syntactically valid for registration or not.
 *
 * \param ctx  operation context
 * \param domain_fqdn  fully qualified domain name
 * \param is_system_registrar  fully qualified domain name
 *
 * \return  domain name syntax validity
 */
DomainFqdnSyntaxValidity::Enum get_domain_fqdn_syntax_validity(
        const OperationContext& ctx,
        const std::string& domain_fqdn,
        bool is_system_registrar = false);

/**
 * \brief  Returns whether the domain identified by its FQDN is generally registrable, blacklisted or not in zone.
 *
 *         Includes very basic "general domain name syntax check".  Should be the domain registered,
 *         advanced FQDN syntax check must be performed in addition to this registrability check.
 *
 *         \sa get_domain_fqdn_syntax_validity
 *
 * \param ctx  operation context
 * \param domain_fqdn  fully qualified domain name
 * \param is_registrar_system  if true, will return results appropriate for system registrar
 *
 * \return  domain registrability status.  Does NOT include advanced FQDN syntax check.
 */
DomainRegistrability::Enum get_domain_registrability_by_domain_fqdn(
        const OperationContext& ctx,
        const std::string& domain_fqdn,
        bool is_registrar_system = false);

}//namespace LibFred::Domain
}//namespace LibFred

#endif//DOMAIN_HH_CD4EA3CAAAB3482BBE7CEA41B4E6FDCA
