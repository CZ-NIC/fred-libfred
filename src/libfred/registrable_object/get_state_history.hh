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

#include "libfred/registrable_object/exceptions.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/state_history.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {

template <typename D, typename S>
class GetStateHistory
{
public:
    static constexpr Object_Type::Enum object_type = S::Tag::object_type;
    using Result = StateHistory<S>;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    const D& derived()const;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATE_HISTORY_HH_E29792C4AA985EF946753B654D199342
