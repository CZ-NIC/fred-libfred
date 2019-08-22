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

#ifndef BOOST_DATE_HH_C936F81F2BA04C00A6915DCAF9B9325D
#define BOOST_DATE_HH_C936F81F2BA04C00A6915DCAF9B9325D

#include "integral.hh"
#include "uniform_distribution.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <utility> // declval

namespace Random {
namespace Algorithm {

template<>
class UniformDistribution<boost::gregorian::date>
{
public:    
    UniformDistribution(const boost::gregorian::date& _min, const boost::gregorian::date& _max);

    // Generate random gregorian date between given boundaries (both inclusive).
    template<typename E>
    boost::gregorian::date operator()(E& _engine)
    {
        const auto offset = offset_generator_(_engine);
        return min_ + boost::gregorian::date_duration(offset);
    }
private:
    using Diff = decltype(
            (std::declval<boost::gregorian::date>() - std::declval<boost::gregorian::date>()).days());

    const boost::gregorian::date min_;
    UniformDistribution<Diff> offset_generator_;
};

} // namespace Random::Algorithm
} // nanespace Random

#endif
