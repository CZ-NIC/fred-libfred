/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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
#ifndef MOCK_GENERATOR_HH_39D54367AD214969A7758E8CEF70D772
#define MOCK_GENERATOR_HH_39D54367AD214969A7758E8CEF70D772

#include <atomic>
#include <cstdint>
#include <limits>

class MockEngine
{
public:
    typedef std::uint_fast64_t result_type;

    explicit MockEngine(result_type _seed = 0);

    template <typename Sseq>
    explicit MockEngine(Sseq&)
    {
        ++number_created_;
        ++number_of_instances_;
    }

    ~MockEngine();

    MockEngine(MockEngine&&) = default; // support moving
    MockEngine& operator=(MockEngine&&) = default;

    MockEngine(const MockEngine&) = default; // support copying
    MockEngine& operator=(const MockEngine&) = default;

    static int get_number_created();

    static int get_number_of_instances();

    static void reset();

    result_type operator()();

    static constexpr result_type min()
    {
        return std::numeric_limits<result_type>::min();
    }

    static constexpr result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

private:
    static std::atomic<int> number_created_;
    static std::atomic<int> number_of_instances_;
};

#endif
