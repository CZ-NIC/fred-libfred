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
#ifndef GET_HANDLE_HISTORY_HH_3047371BA0FB77E8A956C08C3A333431//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_HANDLE_HISTORY_HH_3047371BA0FB77E8A956C08C3A333431

#include "libfred/registrable_object/handle_history.hh"
#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
class GetHandleHistory
{
public:
    GetHandleHistory(const std::string& handle);
    static constexpr Object_Type::Enum object_type = o;
    using Result = HandleHistory<object_type>;
    Result exec(OperationContext& ctx)const;
private:
    std::string handle_;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_HANDLE_HISTORY_HH_3047371BA0FB77E8A956C08C3A333431
