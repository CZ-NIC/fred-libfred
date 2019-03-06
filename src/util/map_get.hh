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
#ifndef MAP_GET_HH_09F4233C0F124E0DA8FDC558CD63AC68
#define MAP_GET_HH_09F4233C0F124E0DA8FDC558CD63AC68

#include <vector>
#include <algorithm>


template<typename T>
struct KeyGetter
{
    typedef typename T::first_type getter_type;

    getter_type operator ()(const T &_pair) const
    {
        return _pair.first;
    }
};



template<typename T>
struct ValueGetter
{
    typedef typename T::second_type getter_type;

    getter_type operator ()(const T &_pair) const
    {
        return _pair.second;
    }
};



template<typename T>
std::vector<typename T::key_type> map_get_keys(const T &_map)
{
    std::vector<typename T::key_type> tmp;
    std::transform(
            _map.begin(),
            _map.end(),
            std::back_inserter(tmp),
            KeyGetter<typename T::value_type>());
    return tmp;
}



template<typename T>
std::vector<typename T::mapped_type> map_get_values(const T &_map)
{
    std::vector<typename T::mapped_type> tmp;
    std::transform(
            _map.begin(),
            _map.end(),
            std::back_inserter(tmp),
            ValueGetter<typename T::value_type>());
    return tmp;
}



#endif /*MAP_GET_H_*/

