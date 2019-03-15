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

#ifndef CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398

#include "util/db/query_param.hh"

#include <stdexcept>
#include <string>
#include <utility>

namespace Util {

struct CaseInsensitive
{
    enum class ComparisonResult
    {
        equal,
        not_equal
    };
    template <typename T>
    static ComparisonResult compare(T&& db_conn, const std::string& lhs, const std::string& rhs)
    {
        switch (std::forward<T>(db_conn).exec_params("SELECT 0 WHERE UPPER($1::TEXT)=UPPER($2::TEXT)",
                                                     Database::query_param_list(lhs)(rhs)).size())
        {
            case 0:
                return ComparisonResult::not_equal;
            case 1:
                return ComparisonResult::equal;
        }
        throw std::runtime_error("unexpected number of results");
    }
    static ComparisonResult compare(const std::string& lhs, const std::string& rhs);
};

}//namespace Util

#endif//CASE_INSENSITIVE_HH_151611729A3F42461E1C9D28A6F1A398
