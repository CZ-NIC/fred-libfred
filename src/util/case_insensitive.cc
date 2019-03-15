/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#include "src/util/case_insensitive.hh"
#include "src/libfred/opcontext.hh"
#include "util/db/query_param.hh"

#include <stdexcept>

namespace Util {

template <typename T>
bool CaseInsensitiveEqualTo<T>::operator()(const std::string& lhs, const std::string& rhs)const
{
    switch (db_conn_.exec_params("SELECT 0 WHERE UPPER($1::TEXT)=UPPER($2::TEXT)",
                                 Database::query_param_list(lhs)(rhs)).size())
    {
        case 0:
            return false;
        case 1:
            return true;
    }
    throw std::runtime_error("unexpected number of results");
}

bool CaseInsensitiveEqualTo<void>::operator()(const std::string& lhs, const std::string& rhs)const
{
    return case_insensitive_equal_to(LibFred::OperationContextCreator().get_conn())(lhs, rhs);
}

CaseInsensitiveEqualTo<void> case_insensitive_equal_to()
{
    return CaseInsensitiveEqualTo<void>();
}

template class CaseInsensitiveEqualTo<std::remove_reference_t<decltype(LibFred::OperationContextCreator().get_conn())>>;

}//namespace Util
