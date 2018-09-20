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

#ifndef OPERATION_BY_UUID_HH_D040ECF44D1F9213A9D20B7375621D94//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define OPERATION_BY_UUID_HH_D040ECF44D1F9213A9D20B7375621D94

#include "util/db/query_param.hh"

#include <string>

namespace LibFred {
namespace RegistrableObject {

template <template <typename, typename> class O, typename T>
class OperationByUUID : public O<OperationByUUID<O, T>, T>
{
public:
    explicit OperationByUUID(const std::string& uuid);
    explicit OperationByUUID(unsigned long long uuid);
private:
    std::string get_object_id_rule(Database::query_param_list& params)const;
    std::string uuid_;
    friend class O<OperationByUUID, T>;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//OPERATION_BY_UUID_HH_D040ECF44D1F9213A9D20B7375621D94
