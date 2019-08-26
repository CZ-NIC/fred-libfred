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

#ifndef RANDOM_HH_F3D4B49BDF6145FE88CE0239126DF4AA
#define RANDOM_HH_F3D4B49BDF6145FE88CE0239126DF4AA

#include "algorithm/integral.hh"
#include "algorithm/random_element.hh"
#include "algorithm/sequence.hh"
#include "engine_management.hh"
#include "seeders/nondeterministic_seeder.hh"

#include <algorithm> // std::shuffle
#include <cstddef> // std::size_t
#include <random> // std::mt19937_64

namespace Random {

template<typename I, typename E, typename S>
class BasicGenerator
{
public:
    BasicGenerator()
        : engine_(I::template get_engine<E, S>())
    { }

    template<typename T>
    T get(const T& _min, const T& _max)
    {
        return Algorithm::UniformDistribution<T>(_min, _max)(engine_);
    }

    template<typename T, std::size_t N>
    T get(const T (&_source_set)[N]) 
    {
        return Algorithm::RandomElementSelector<T>(_source_set)(engine_);
    }

    template<typename T, std::size_t N>
    auto get_seq(const T (&_source_set)[N], std::size_t _result_size)
    {
        return Algorithm::SequenceGenerator<T>(_source_set, _result_size)(engine_);
    }

    template<typename RandomIt>
    void shuffle(RandomIt first, RandomIt last)
    {
        std::shuffle(first, last, engine_);
    }
private:
    decltype(I::template get_engine<E, S>()) engine_;
};

using DefaultEngine = std::mt19937_64;

using Generator = BasicGenerator<ThreadLocalEngineInstance, DefaultEngine, Seeders::NondeterministicSeeder>;

} // namespace Random

#endif
