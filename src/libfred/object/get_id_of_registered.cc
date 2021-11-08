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
#include "libfred/object/get_id_of_registered.hh"
#include "libfred/db_settings.hh"

#include <stdexcept>

namespace LibFred {

const char* UnknownObject::what()const noexcept
{
    return "unknown registry object type or handle";
}

namespace {

enum CaseSensitivity
{
    lowercase,
    uppercase
};

template <Object_Type::Enum object_type>
struct ObjectTypeTraits
{
    static const CaseSensitivity handle_case_sensitivity = uppercase;//contacts, nssets and keysets have uppercase handles
};

template < >
struct ObjectTypeTraits<Object_Type::domain>
{
    static const CaseSensitivity handle_case_sensitivity = lowercase;//only domains have lowercase handles
};

template <CaseSensitivity case_sensitivity>
const char* get_sql_conversion_function();

template < >
const char* get_sql_conversion_function<lowercase>() { return "LOWER"; }

template < >
const char* get_sql_conversion_function<uppercase>() { return "UPPER"; }

} // namespace LibFred::{anonymous}

template < Object_Type::Enum object_type >
unsigned long long get_id_of_registered(
        const OperationContext& ctx,
        const std::string& handle)
{
    static const std::string normalize_function =
            get_sql_conversion_function< ObjectTypeTraits<object_type>::handle_case_sensitivity >();
    static const std::string sql =
            "SELECT id "
            "FROM object_registry "
            "WHERE type=get_object_type_id($1::TEXT) AND "
                  "name=" + normalize_function + "($2::TEXT) AND "
                  "erdate IS NULL";
    static const std::string object_type_handle = Conversion::Enums::to_db_handle(object_type);
    const Database::Result dbres = ctx.get_conn().exec_params(
            sql,
            Database::query_param_list(object_type_handle)(handle));
    if (dbres.size() < 1)
    {
        throw UnknownObject();
    }
    if (1 < dbres.size())
    {
        throw std::runtime_error("too many objects for given handle and type");
    }
    return static_cast<unsigned long long>(dbres[0][0]);
}

template unsigned long long get_id_of_registered<Object_Type::contact>(const OperationContext& , const std::string&);
template unsigned long long get_id_of_registered<Object_Type::nsset>(const OperationContext& , const std::string&);
template unsigned long long get_id_of_registered<Object_Type::domain>(const OperationContext& , const std::string&);
template unsigned long long get_id_of_registered<Object_Type::keyset>(const OperationContext& , const std::string&);

} // namespace LibFred
