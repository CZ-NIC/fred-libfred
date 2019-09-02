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

#ifndef NONDETERMINISTIC_SEEDER_HH_AE5D1C96197B4857AC61A2279A8315D0
#define NONDETERMINISTIC_SEEDER_HH_AE5D1C96197B4857AC61A2279A8315D0

#include <cstdint>
#include <random>
#include <vector>

namespace Random {
namespace Seeders {

class NondeterministicSeeder
{
public:
    NondeterministicSeeder();

    std::seed_seq& get_seed();
private:
    // auxiliary value only used during construction
    std::vector<std::uint32_t> intermediate_seeds_;

    std::seed_seq seed_;
};

} // namespace Random::Seeders
} // namespace Random

#endif
