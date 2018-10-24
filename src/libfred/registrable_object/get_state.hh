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

#ifndef GET_STATE_HH_8E9BA9BBE7F8A87FBC864909C2347D66//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_STATE_HH_8E9BA9BBE7F8A87FBC864909C2347D66

#include "libfred/registrable_object/exceptions.hh"
#include "libfred/registrable_object/state.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {

template <typename D, typename S>
class GetState
{
public:
    static constexpr Object_Type::Enum object_type = S::Tag::object_type;
    using Result = S;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(OperationContext& ctx)const;
private:
    const D& derived()const;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATE_HH_8E9BA9BBE7F8A87FBC864909C2347D66
