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

#ifndef GET_STATUS_HH_8E9BA9BBE7F8A87FBC864909C2347D66//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_STATUS_HH_8E9BA9BBE7F8A87FBC864909C2347D66

#include "libfred/registrable_object/status.hh"
#include "libfred/opcontext.hh"

#include <stdexcept>
#include <string>
#include <type_traits>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
struct ObjectNotFound : std::runtime_error
{
    static constexpr Object_Type::Enum object_type = o;
    ObjectNotFound();
};

template <typename, typename T> bool does_correspond(const T&);

template <typename D, typename S>
class GetStatus
{
public:
    static constexpr Object_Type::Enum object_type = S::object_type;
    using Result = S;
    using NotFound = ObjectNotFound<object_type>;
    Result exec(OperationContext& ctx)const;
private:
    const D& derived()const;
    template <typename ...Fs>
    class StatusFlagSetter;
};

template <typename S>
class GetStatusById : public GetStatus<GetStatusById<S>, S>
{
public:
    explicit GetStatusById(unsigned long long object_id);
private:
    std::string get_object_id_rule(Database::query_param_list& params)const;
    unsigned long long object_id_;
    friend class GetStatus<GetStatusById, S>;
};

template <typename S>
class GetStatusByHandle : public GetStatus<GetStatusByHandle<S>, S>
{
public:
    explicit GetStatusByHandle(const std::string& handle);
private:
    std::string get_object_id_rule(Database::query_param_list& params)const;
    std::string handle_;
    friend class GetStatus<GetStatusByHandle, S>;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATUS_HH_8E9BA9BBE7F8A87FBC864909C2347D66
