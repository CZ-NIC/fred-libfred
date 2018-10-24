/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HANDLE_HISTORY_HH_DEF68B0929AD081E5593338F8E674DD1//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define HANDLE_HISTORY_HH_DEF68B0929AD081E5593338F8E674DD1

#include "libfred/object/object_type.hh"

#include <boost/optional.hpp>

#include <chrono>
#include <string>
#include <vector>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
struct HandleHistory
{
    static constexpr Object_Type::Enum object_type = o;
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
    struct Record
    {
        unsigned long long id;
        struct
        {
            TimePoint timestamp;
            unsigned long long history_id;
        } begin;
        struct
        {
            boost::optional<TimePoint> timestamp;
            unsigned long long history_id;
        } end;
    };
    std::string handle;
    std::vector<Record> timeline;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//HANDLE_HISTORY_HH_DEF68B0929AD081E5593338F8E674DD1
