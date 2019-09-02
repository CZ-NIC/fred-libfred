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

#ifndef INTEGRAL_HH_1153227B70DE483BA11B888C204AA160
#define INTEGRAL_HH_1153227B70DE483BA11B888C204AA160

#include "uniform_distribution.hh"

#include <random>
#include <stdexcept>
#include <type_traits>

namespace Random {
namespace Algorithm {

template<typename T>
class UniformDistribution<T, std::enable_if_t<std::is_integral<T>::value>>
{
public:
    UniformDistribution(T _min, T _max)
        : generator_(make_interval(_min, _max))
    { }

    template<typename E>
    T operator()(E& _engine)
    {
        return generator_(_engine);
    }

private:
    std::uniform_int_distribution<T> generator_;

    static auto make_interval(T _min, T _max)
    {
        if (_min > _max)
        {
            throw std::invalid_argument("incorrect bounds order");
        }
        return typename std::uniform_int_distribution<T>::param_type(_min, _max);
    }
};

} // namespace Random::Algorithm
} // nanespace Random

#endif
