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

#include "util/optional_value.hh"
#include "libfred/registrable_object/contact/find_contact_duplicates.hh"

#include <boost/algorithm/string/join.hpp>

#include <sstream>
#include <string>

namespace LibFred {
namespace Contact {

FindContactDuplicates::FindContactDuplicates()
{
}

FindContactDuplicates& FindContactDuplicates::set_registrar(const Optional<std::string>& _registrar_handle)
{
    registrar_handle_ = _registrar_handle;
    return *this;
}

FindContactDuplicates& FindContactDuplicates::set_exclude_contacts(const std::set<std::string>& _exclude_contacts)
{
    exclude_contacts_ = _exclude_contacts;
    return *this;
}

std::set<std::string> FindContactDuplicates::exec(const LibFred::OperationContext& _ctx)
{
    std::set<std::string> result;
    Database::QueryParams dup_params;
    std::vector<std::string> contact_address_types = Util::vector_of<std::string>
            ("MAILING")
            ("BILLING")
            ("SHIPPING")
            ("SHIPPING_2")
            ("SHIPPING_3");
    std::ostringstream dup_sql;
    dup_sql << "SELECT unnest(dup_set)"
        " FROM (SELECT array_agg(oreg.name) AS dup_set,";
        for (std::vector<std::string>::const_iterator contact_address_type = contact_address_types.begin();
            contact_address_type != contact_address_types.end();
            ++contact_address_type)
        {
            dup_sql <<
            " (SELECT row("
               "COALESCE(LOWER(REGEXP_REPLACE(ca.company_name, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.street1, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.street2, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.street3, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.city, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.stateorprovince, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.postalcode, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
               "COALESCE(LOWER(REGEXP_REPLACE(ca.country, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), '')) "
              "FROM contact_address ca"
             " WHERE ca.type = '" << *contact_address_type << "'"
               " AND ca.contactid = c.id"
            " ) AS " << *contact_address_type << "_addr" << (contact_address_type != contact_address_types.end() - 1 ? "," : "");
        }
    dup_sql <<
        " FROM object_registry oreg"
        " JOIN contact c ON c.id = oreg.id"
        " JOIN object o ON o.id = c.id"
        " JOIN registrar r ON r.id = o.clid";

    if (registrar_handle_.isset()) {
        dup_params.push_back(registrar_handle_.get_value());
        dup_sql << " WHERE r.handle = $" << dup_params.size() << "::text";
    }

    dup_sql << " GROUP BY "
        "COALESCE(LOWER(REGEXP_REPLACE(c.name, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.organization, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(CAST(c.ssntype AS TEXT), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.ssn, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.vat, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.telephone, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.fax, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.email, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.notifyemail, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.street1, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.street2, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.street3, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.city, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.stateorprovince, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.postalcode, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "COALESCE(LOWER(REGEXP_REPLACE(c.country, '^\\s+|\\s+$|(\\s)\\s+', '\\1', 'g')), ''), "
        "o.clid, "
        "c.disclosename, "
        "c.discloseorganization, "
        "c.discloseaddress, "
        "c.disclosetelephone, "
        "c.disclosefax, "
        "c.discloseemail, "
        "c.disclosevat, "
        "c.discloseident, "
        "c.disclosenotifyemail, "
        "COALESCE(CAST(c.warning_letter AS TEXT), ''),";
    for (std::vector<std::string>::const_iterator contact_address_type = contact_address_types.begin();
        contact_address_type != contact_address_types.end();
        ++contact_address_type)
    {
        dup_sql <<
        " " << *contact_address_type << "_addr" << (contact_address_type != contact_address_types.end() - 1 ? "," : "");
    }
    dup_sql << \
        " HAVING array_upper(array_agg(oreg.name), 1) > 1";
    if (!exclude_contacts_.empty()) {
        std::vector<std::string> array_params;
        for (std::set<std::string>::const_iterator i = exclude_contacts_.begin();
                i != exclude_contacts_.end(); ++i)
        {
            dup_params.push_back(*i);
            array_params.push_back("$" + boost::lexical_cast<std::string>(dup_params.size()));
        }

        dup_sql << " AND NOT (array_agg(oreg.name)"
                << " && array[" << boost::algorithm::join(array_params, ", ") << "]::varchar[])";
    }
    dup_sql << " LIMIT 1) as dup_q";

    const Database::Result dup_result = _ctx.get_conn().exec_params(dup_sql.str(), dup_params);

    for (Database::Result::size_type i = 0; i < dup_result.size(); i++) {
        result.insert(static_cast<std::string>(dup_result[i][0]));
    }

    return result;
}

}//namespace LibFred::Contact
}//namespace LibFred
