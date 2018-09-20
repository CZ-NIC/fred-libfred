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

#ifndef OPERATION_BY_HANDLE_IMPL_HH_3CA5FA186ADDECF08FFC5228EA1F8808//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define OPERATION_BY_HANDLE_IMPL_HH_3CA5FA186ADDECF08FFC5228EA1F8808

#include "libfred/registrable_object/operation_by_handle.hh"

namespace LibFred {
namespace RegistrableObject {

template <template <typename, typename> class O, typename T>
OperationByHandle<O, T>::OperationByHandle(const std::string& handle)
    : handle_(handle)
{ }

template <template <typename, typename> class O, typename T>
std::string OperationByHandle<O, T>::get_object_id_rule(Database::query_param_list& params)const
{
    static const std::string sql_handle_case_normalize_function =
            T::Tag::object_type == Object_Type::domain ? "LOWER"
                                                       : "UPPER";
    return "SELECT id "
           "FROM object_registry "
           "WHERE name=" + sql_handle_case_normalize_function + "($" + params.add(handle_) + "::TEXT) AND "
                 "type=get_object_type_id($" + params.add(Conversion::Enums::to_db_handle(T::Tag::object_type)) + "::TEXT) AND "
                 "erdate IS NULL";
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//OPERATION_BY_HANDLE_IMPL_HH_3CA5FA186ADDECF08FFC5228EA1F8808
