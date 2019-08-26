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

#include "mock_seeder.hh"

#include <atomic>
#include <cstdint>

MockSeeder::MockSeeder()
{
    ++number_created_;
}

std::uint32_t MockSeeder::get_seed() const
{
    return 42;
}

int MockSeeder::get_number_created()
{
    return number_created_;
}

void MockSeeder::reset()
{
    number_created_ = 0;
}

std::atomic<int> MockSeeder::number_created_(0);
