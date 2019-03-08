/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
#include <boost/algorithm/string/trim.hpp>
#include <vector>

#include "util/db/value.hh"
#include "util/string_split.hh"


namespace Database {


std::vector<std::string> array_to_vector(std::string _dbarr)
{
    boost::algorithm::trim(_dbarr);
    if ((*(_dbarr.begin()) == '{') && (*(_dbarr.end() - 1) == '}'))
    {
        _dbarr.erase(_dbarr.begin());
        _dbarr.erase(_dbarr.end() -1);
    }
    else
    {
        throw std::runtime_error("not a database array value");
    }

    return split<std::string>(_dbarr, ",");
}


}

