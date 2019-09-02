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

#ifndef RANDOM_ELEMENT_HH_D13305AB434A4343B716985350C1682E
#define RANDOM_ELEMENT_HH_D13305AB434A4343B716985350C1682E

#include "integral.hh"
#include "util/random/details/utils.hh"

#include <cstddef>
#include <type_traits>

namespace Random {
namespace Algorithm {

template<typename T>
class RandomElementSelector
{
public:
    // Warning: This constructor is only appropriate when the _source_set
    // array has lifetime longer than or equal to that of this object.
    template<std::size_t N,
             // Compensate for null termination byte in a string.
             bool null_terminated = Random::Details::IsChar<T>::value>
    RandomElementSelector(const T (&_source_set)[N])
        : src_begin_(_source_set),
          offset_generator_(std::size_t(0), null_terminated ? N - 2 : N - 1)
    {
        static_assert(null_terminated ? (N > 1) : (N > 0), "zero array size");
    }

    template<typename E>
    T operator()(E& _engine)
    {
        return src_begin_[offset_generator_(_engine)];
    }
private:
    const T* src_begin_;
    UniformDistribution<std::size_t> offset_generator_;
};

} // namespace Random::Algorithm
} // namespace Random

#endif
