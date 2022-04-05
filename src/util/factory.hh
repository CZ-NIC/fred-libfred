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

#ifndef FACTORY_HH_8EED84798BF84FCCB6953709F67F7BF2
#define FACTORY_HH_8EED84798BF84FCCB6953709F67F7BF2

#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <utility>


namespace Util {

/*
 * Abstract factory template class
 *
 * \tparam Producer  the base class from which a particular implementation class is derived
 * \tparam Key       the type of key used to register a particular implementation class in the factory
 */
template <typename Producer, typename Key = std::string>
class Factory
{
public:
    using ProducerInterface = Producer;
    using KeyType = Key;
    using KeyProducerMap = std::map<KeyType, std::unique_ptr<ProducerInterface>>;
    using KeyProducerPair = std::pair<KeyType, std::unique_ptr<ProducerInterface>>;

    Factory() = default;
    Factory(Factory&&) = default;
    Factory& operator=(Factory&&) = default;

    Factory(const Factory&) = delete;
    Factory& operator=(const Factory&) = delete;

    Factory& add_producer(KeyProducerPair key_producer_pair)
    {
        const bool is_unique = key_producer_map_.insert(std::move(key_producer_pair)).second;
        if (!is_unique)
        {
            throw std::runtime_error{"uniqueness constraint violation"};
        }
        return *this;
    }
    bool has_key(const KeyType& key) const noexcept
    {
        return key_producer_map_.find(key) != std::end(key_producer_map_);
    }
    ProducerInterface& operator[](const KeyType& key) const
    {
        const auto key_producer_iter = key_producer_map_.find(key);
        if (key_producer_iter != std::end(key_producer_map_))
        {
            return *(key_producer_iter->second);
        }
        throw std::out_of_range{"producer not registered"};
    }
private:
    KeyProducerMap key_producer_map_;
    friend auto begin(const Factory<Producer, Key>& factory)
    {
        return std::begin(factory.key_producer_map_);
    }
    friend auto end(const Factory<Producer, Key>& factory)
    {
        return std::end(factory.key_producer_map_);
    }
};

}//namespace Util

#endif//FACTORY_HH_8EED84798BF84FCCB6953709F67F7BF2
