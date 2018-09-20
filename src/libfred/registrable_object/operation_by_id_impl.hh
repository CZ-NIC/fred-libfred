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

#ifndef OPERATION_BY_ID_IMPL_HH_FBA42FFC1F3FD34A6DEB6CB7F59A3CFF//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define OPERATION_BY_ID_IMPL_HH_FBA42FFC1F3FD34A6DEB6CB7F59A3CFF

#include "libfred/registrable_object/operation_by_id.hh"

#include <string>
#include <type_traits>

namespace LibFred {
namespace RegistrableObject {

template <template <typename, typename> class O, typename T>
OperationById<O, T>::OperationById(unsigned long long object_id)
    : object_id_(object_id)
{ }

template <template <typename, typename> class O, typename T>
std::string OperationById<O, T>::get_object_id_rule(Database::query_param_list& params)const
{
    return "$" + params.add(object_id_) + "::BIGINT";
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//OPERATION_BY_ID_IMPL_HH_FBA42FFC1F3FD34A6DEB6CB7F59A3CFF
