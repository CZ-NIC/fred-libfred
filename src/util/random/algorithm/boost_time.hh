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

#ifndef BOOST_TIME_HH_11C154B1329D4D8AAD13F88212E72E2B
#define BOOST_TIME_HH_11C154B1329D4D8AAD13F88212E72E2B

#include "integral.hh"
#include "uniform_distribution.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <utility> // std::declval

namespace Random {
namespace Algorithm {

template<>
class UniformDistribution<boost::posix_time::ptime>
{
public:
    UniformDistribution(const boost::posix_time::ptime& _min, const boost::posix_time::ptime& _max);

    // Generate random POSIX time between given boundaries (both inclusive),
    // with microsecond precision.
    template<typename E>
    boost::posix_time::ptime operator()(E& _engine)
    {
        const auto offset = offset_generator_(_engine);
        return min_ + boost::posix_time::microseconds(offset);
    }
private:
    using Diff = decltype(
            (std::declval<boost::posix_time::ptime>() - std::declval<boost::posix_time::ptime>())
                .total_microseconds());

    const boost::posix_time::ptime min_;
    UniformDistribution<Diff> offset_generator_;
};

} // namespace Random::Algorithm
} // nanespace Random

#endif
