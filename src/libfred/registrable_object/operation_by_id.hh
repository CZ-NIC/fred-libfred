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

#ifndef OPERATION_BY_ID_HH_3F8489F4069FC16E4EEFF1AAE605425F//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define OPERATION_BY_ID_HH_3F8489F4069FC16E4EEFF1AAE605425F

#include "util/db/query_param.hh"

namespace LibFred {
namespace RegistrableObject {

template <template <typename, typename> class O, typename T = void>
class OperationById : public O<OperationById<O, T>, T>
{
public:
    explicit OperationById(unsigned long long object_id);
private:
    std::string get_object_id_rule(Database::query_param_list& params)const;
    unsigned long long object_id_;
    friend class O<OperationById, T>;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//OPERATION_BY_ID_HH_3F8489F4069FC16E4EEFF1AAE605425F
