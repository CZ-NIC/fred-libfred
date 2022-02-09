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
#include "mock_engine.hh"

#include <atomic>
#include <limits>

MockEngine::MockEngine(MockEngine::result_type)
{
    ++number_created_;
    ++number_of_instances_;
}

MockEngine::~MockEngine()
{
    --number_of_instances_;
}

int MockEngine::get_number_created()
{
    return number_created_;
}

int MockEngine::get_number_of_instances()
{
    return number_of_instances_;
}

void MockEngine::reset()
{
    number_created_ = 0;
    number_of_instances_ = 0;
}

MockEngine::result_type MockEngine::operator()()
{
    return 0;
}

std::atomic<int> MockEngine::number_created_(0);
std::atomic<int> MockEngine::number_of_instances_(0);
