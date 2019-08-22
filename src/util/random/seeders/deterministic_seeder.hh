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

#ifndef DETERMINISTIC_SEEDER_HH_DC8FC9FB249342D8B1F81BF667E1F953
#define DETERMINISTIC_SEEDER_HH_DC8FC9FB249342D8B1F81BF667E1F953

#include <cstdint>

namespace Random {
namespace Seeders {

template<std::uint32_t custom_seed>
struct CustomDeterministicSeeder
{
    using Seed = std::uint32_t;

    Seed get_seed() const
    {
        return custom_seed;
    }
};

using DeterministicSeeder = CustomDeterministicSeeder<0>;

} // namespace Random::Seeders
} // namespace Random

#endif
