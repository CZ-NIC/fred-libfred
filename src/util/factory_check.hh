/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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
#ifndef FACTORY_CHECK_HH_32E8EF58E9B844A89ABA5459C1AA8D6A
#define FACTORY_CHECK_HH_32E8EF58E9B844A89ABA5459C1AA8D6A

#include "util/factory.hh"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace Util {

template <typename Producer, typename Key>
bool is_each_factory_key_in_superset(const Factory<Producer, Key>& factory, const std::vector<Key>& superset)
{
    const auto key_not_in_superset = std::find_if_not(
            begin(factory),
            end(factory),
            [&](auto&& key_producer_pair)
            {
                return std::find(begin(superset), end(superset), key_producer_pair.first) != end(superset);
            });
    return key_not_in_superset == end(factory);
}

template <typename Producer, typename Key>
bool is_each_key_in_factory(const Factory<Producer, Key>& factory, const std::vector<Key>& subset)
{
    const auto key_not_in_factory = std::find_if_not(
            begin(subset),
            end(subset),
            [&](auto&& key)
            {
                return std::find_if(begin(factory), end(factory), [&](auto&& key_producer_pair) { return key_producer_pair.first == key; }) != end(factory);
            });
    return key_not_in_factory == end(subset);
}

struct FactoryHaveSubsetOfKeys
{
    template <typename Producer, typename Key>
    static void require(const Factory<Producer, Key>& factory, const std::vector<Key>& superset)
    {
        if (!is_each_factory_key_in_superset(factory, superset))
        {
            throw std::runtime_error{"FactoryHaveSubsetOfKeys: at least one key from a Factory not found in a superset of keys"};
        }
    }
};

struct FactoryHaveSupersetOfKeys
{
    template <typename Producer, typename Key>
    static void require(const Factory<Producer, Key>& factory, const std::vector<Key>& subset)
    {
        if (!is_each_key_in_factory(factory, subset))
        {
            throw std::runtime_error{"FactoryHaveSupersetOfKeys: at least one key from a subset not found in a Factory"};
        }
    }
};

//get implementation names begining with prefix from factory
template <typename Base, typename Key>
std::vector<std::string> get_names_begining_with_prefix(const std::string& prefix, const Factory<Base, Key>& factory)
{
    std::vector<std::string> names;
    std::for_each(begin(factory), end(factory), [&](auto&& key_producer_pair)
    {
        if (key_producer_pair.first.compare(0, prefix.length(), prefix) == 0)
        {
            names.push_back(key_producer_pair.first);
        }
    });
    return names;
}

}//namespace Util

#endif
