/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPERATION_BY_UUID_IMPL_HH_D5809AF0BAD7A6F7806DD1C5FA5B1B49//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define OPERATION_BY_UUID_IMPL_HH_D5809AF0BAD7A6F7806DD1C5FA5B1B49

#include "libfred/registrable_object/operation_by_uuid.hh"

#include <boost/lexical_cast.hpp>

namespace LibFred {
namespace RegistrableObject {

template <template <typename, typename> class O, typename T>
OperationByUUID<O, T>::OperationByUUID(const std::string& uuid)
    : uuid_(uuid)
{ }

template <template <typename, typename> class O, typename T>
OperationByUUID<O, T>::OperationByUUID(unsigned long long uuid)
    : uuid_(boost::lexical_cast<std::string>(uuid))
{ }

template <template <typename, typename> class O, typename T>
std::string OperationByUUID<O, T>::get_object_id_rule(Database::query_param_list& params)const
{
    static const std::string sql_handle_case_normalize_function =
            T::Tag::object_type == Object_Type::domain ? "LOWER"
                                                       : "UPPER";
    const auto object_type_param_text = "$" + params.add(Conversion::Enums::to_db_handle(T::Tag::object_type)) + "::TEXT";
    try
    {
        const unsigned long long uuid = boost::lexical_cast<unsigned long long>(uuid_);
        const auto uuid_param = "$" + params.add(uuid);
        return "SELECT id "
               "FROM object_registry "
               "WHERE (name=" + sql_handle_case_normalize_function + "(" + uuid_param + "::TEXT) AND "
                      "type=get_object_type_id(" + object_type_param_text + ") AND "
                      "erdate IS NULL) OR "
                     "id=" + uuid_param + "::BIGINT "
               "LIMIT 1";
    }
    catch (...)
    {
        const auto uuid_param = "$" + params.add(uuid_);
        return "SELECT id "
               "FROM object_registry "
               "WHERE name=" + sql_handle_case_normalize_function + "(" + uuid_param + "::TEXT) AND "
                     "type=get_object_type_id(" + object_type_param_text + ") AND "
                     "erdate IS NULL";
    }
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//OPERATION_BY_UUID_IMPL_HH_D5809AF0BAD7A6F7806DD1C5FA5B1B49
