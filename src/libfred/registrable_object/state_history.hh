/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
#ifndef STATE_HISTORY_HH_66DEDB2996A3E6D6855BF12D348DF062//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STATE_HISTORY_HH_66DEDB2996A3E6D6855BF12D348DF062

#include <chrono>
#include <vector>

namespace LibFred {
namespace RegistrableObject {

template <typename S>
struct StateHistory
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
    struct Record
    {
        TimePoint valid_from;
        S state;
    };
    std::vector<Record> timeline;
    TimePoint valid_to;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//STATE_HISTORY_HH_66DEDB2996A3E6D6855BF12D348DF062
