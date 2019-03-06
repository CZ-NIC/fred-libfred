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
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>


template<class T>
std::vector<T> split(const std::string &_str, const char *_separators)
{
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(_separators);

    std::vector<T> output;
    tokenizer t(_str, sep);
    for (tokenizer::iterator it = t.begin(); it != t.end(); ++it)
    {
        output.push_back(boost::lexical_cast<T>(*it));
    }
    return output;
}

