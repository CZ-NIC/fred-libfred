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
/**
 *  @file
 *  print description of object data to the string for debugging and error handling
 */

#include "util/printable.hh"
#include "util/util.hh"

#include <string>
#include <vector>
#include <utility>

namespace Util {

namespace {

class SeparatorWithDataIntoStringFunctor
{
public:
    SeparatorWithDataIntoStringFunctor(std::string& s, const std::string& separator)
        : s_(s),
          separator_(separator)
    {}
    void operator()(const std::pair<std::string, std::string>& string_pair_to_be_added_into_string)
    {
        if (!string_pair_to_be_added_into_string.first.empty())
        {
            s_ += separator_;
            s_ += string_pair_to_be_added_into_string.first;
            s_ += ":";
        }
        if (!string_pair_to_be_added_into_string.second.empty())
        {
            s_ += separator_;
            s_ += string_pair_to_be_added_into_string.second;
        }
    }
private:
    std::string& s_;
    const std::string separator_;
};

}//namespace Util::{anonymous}

std::string format_operation_state(const std::string& operation_name,
    const std::vector<std::pair<std::string, std::string>>& key_value_list)
{
    std::string s;
    SeparatorWithDataIntoStringFunctor add_to_ss(s, " ");

    s += "#";
    s += operation_name;
    std::for_each(key_value_list.begin(), key_value_list.end(), add_to_ss);
    return s;
}

std::string format_data_structure(const std::string& data_structure_name,
    const std::vector<std::pair<std::string, std::string>>& key_value_list)
{
    std::string s;
    SeparatorWithDataIntoStringFunctor add_to_ss(s, " ");

    s += " ";
    s += data_structure_name;
    std::for_each(key_value_list.begin(), key_value_list.end(), add_to_ss);
    return s;
}

}//namespace Util
