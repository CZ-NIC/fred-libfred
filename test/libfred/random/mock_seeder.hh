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

#ifndef MOCK_SEEDER_HH_F2A2FA6A2E034222B3B2B48FC2613C8A
#define MOCK_SEEDER_HH_F2A2FA6A2E034222B3B2B48FC2613C8A

#include <atomic>
#include <cstdint>

class MockSeeder
{
public:
    MockSeeder();

    static int get_number_created();

    static void reset();

    std::uint32_t get_seed() const;

private:
    static std::atomic<int> number_created_;
};

#endif
