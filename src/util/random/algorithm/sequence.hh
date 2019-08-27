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

#ifndef SEQUENCE_HH_FCD2CA98ABC647C6803D1590B70899E1
#define SEQUENCE_HH_FCD2CA98ABC647C6803D1590B70899E1

#include "integral.hh"
#include "random_element.hh"
#include "uniform_distribution.hh"
#include "util/random/details/utils.hh"

#include <cstddef>
#include <string>
#include <vector>

namespace Random {
namespace Algorithm {

template<
    typename T, 
    typename Sequence = std::conditional_t<
        Random::Details::IsChar<T>::value,
        std::basic_string<T>, 
        std::vector<T>>
    >
class SequenceGenerator
{
public:
    // Warning: This constructor is only appropriate when the _source_set
    // array has lifetime longer than or equal to that of this object.
    template<std::size_t N>
    SequenceGenerator(const T (&_source_set)[N], std::size_t _result_size)
        : selector_(_source_set), result_size_(_result_size)
    { }

    template<typename E>
    auto operator()(E& _engine)
    {
        Sequence seq;
        seq.reserve(result_size_);
        for (std::size_t i = 0; i < result_size_; ++i)
        {
            seq.push_back(selector_(_engine));
        }
        return seq;
    }
private:
    RandomElementSelector<T> selector_;
    const std::size_t result_size_;
};

} // namespace Random::Algorithm
} // namespace Random

#endif
