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

#ifndef GET_DATA_HISTORY_HH_C91639AF5F291ABC4A1B0E383ED6BED1//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_DATA_HISTORY_HH_C91639AF5F291ABC4A1B0E383ED6BED1

#include "libfred/registrable_object/data_history.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace RegistrableObject {

template <typename D, typename H>
class GetDataHistory
{
public:
    static constexpr Object_Type::Enum object_type = H::object_type;
    using Result = H;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    const D& derived()const;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_DATA_HISTORY_HH_C91639AF5F291ABC4A1B0E383ED6BED1
