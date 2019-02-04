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

#ifndef GET_STATE_HISTORY_HH_E29792C4AA985EF946753B654D199342//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_STATE_HISTORY_HH_E29792C4AA985EF946753B654D199342

#include "libfred/registrable_object/get_state.hh"

#include <boost/variant.hpp>

#include <chrono>
#include <string>
#include <type_traits>
#include <vector>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
struct InvalidHistoryIntervalSpecification : std::runtime_error
{
    static constexpr Object_Type::Enum object_type = o;
    InvalidHistoryIntervalSpecification();
};

struct HistoryInterval
{
    template <typename R>
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, R>;
    struct HistoryId
    {
        explicit HistoryId(unsigned long long history_id) : value(history_id) { }
        unsigned long long value;
    };
    struct NoLimit { };
    using Limit = boost::variant<TimePoint<std::chrono::nanoseconds>,
                                 HistoryId,
                                 NoLimit>;
    template <typename>
    struct NamedLimit
    {
        template <typename T>
        explicit NamedLimit(const T& src) : value(src)
        {
            static_assert(std::is_same<T, HistoryId>::value ||
                          std::is_same<T, NoLimit>::value,
                          "unsupported conversion requested");
        }
        template <typename R>
        explicit NamedLimit(const TimePoint<R>& src)
            : value(TimePoint<std::chrono::nanoseconds>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(src.time_since_epoch())))
        { }
        Limit value;
    };
    using LowerLimit = NamedLimit<struct Lower>;
    using UpperLimit = NamedLimit<struct Upper>;
    HistoryInterval(
            const LowerLimit& _lower_limit,
            const UpperLimit& _upper_limit)
        : lower_limit(_lower_limit.value),
          upper_limit(_upper_limit.value)
    { }
    Limit lower_limit;
    Limit upper_limit;
};

template <typename S>
struct StateHistory
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
    struct Record
    {
        TimePoint valid_from;
        S state;
    };
    std::vector<Record> record;
    TimePoint valid_to;
};

template <typename D, typename S>
class GetStateHistory
{
public:
    static constexpr Object_Type::Enum object_type = S::Tag::object_type;
    using Result = StateHistory<S>;
    using NotFound = ObjectNotFound<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    const D& derived()const;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATE_HISTORY_HH_E29792C4AA985EF946753B654D199342
