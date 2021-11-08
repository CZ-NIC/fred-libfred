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
#ifndef FIND_CONTACT_DUPLICATES_HH_A345269BAE67408088D69FD24DD3B37E
#define FIND_CONTACT_DUPLICATES_HH_A345269BAE67408088D69FD24DD3B37E

#include "libfred/opcontext.hh"
#include "util/optional_value.hh"

#include <set>
#include <string>


namespace LibFred {
namespace Contact {

class FindContactDuplicates
{
public:
    FindContactDuplicates();

    FindContactDuplicates& set_registrar(const Optional<std::string>& _registrar_handle);
    FindContactDuplicates& set_exclude_contacts(const std::set<std::string>& _exclude_contacts);
    FindContactDuplicates& set_specific_contact(const std::string& _dest_contact_handle);

    std::set<std::string> exec(const LibFred::OperationContext& _ctx);

private:
    Optional<std::string> registrar_handle_;
    std::set<std::string> exclude_contacts_;
    Optional<std::string> specific_contact_handle_;
};

}
}

#endif /*FIND_CONTACT_DUPLICATES*/
