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

#include "boost_date.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace Random {
namespace Algorithm {

UniformDistribution<boost::gregorian::date>::UniformDistribution(
        const boost::gregorian::date& _min,
        const boost::gregorian::date& _max)
    : min_(_min),
      offset_generator_(static_cast<Diff>(0), (_max - _min).days())
{ }

} // namespace Random::Algorithm
} // nanespace Random

