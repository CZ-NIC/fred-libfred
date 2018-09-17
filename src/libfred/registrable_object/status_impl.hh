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

#ifndef STATUS_IMPL_HH_7175EB6FC6ECBA7EE3E85161154E3603//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STATUS_IMPL_HH_7175EB6FC6ECBA7EE3E85161154E3603

#include "libfred/registrable_object/status.hh"

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o, typename ...Flags>
bool Status<o, Flags...>::all()const noexcept
{
    return flags_.all();
}

template <Object_Type::Enum o, typename ...Flags>
bool Status<o, Flags...>::any()const noexcept
{
    return flags_.any();
}

template <Object_Type::Enum o, typename ...Flags>
bool Status<o, Flags...>::none()const noexcept
{
    return flags_.none();
}

template <Object_Type::Enum o, typename ...Flags>
std::size_t Status<o, Flags...>::count()const noexcept
{
    return flags_.count();
}

template <Object_Type::Enum o, typename ...Flags>
constexpr std::size_t Status<o, Flags...>::size()noexcept
{
    return FlagsStorage().size();
}

template <Object_Type::Enum o, typename ...Flags>
Status<o, Flags...>& Status<o, Flags...>::set_all(bool value)noexcept
{
    if (value)
    {
        return this->set_all();
    }
    return this->reset_all();
}

template <Object_Type::Enum o, typename ...Flags>
Status<o, Flags...>& Status<o, Flags...>::set_all()noexcept
{
    flags_.set();
    return *this;
}

template <Object_Type::Enum o, typename ...Flags>
Status<o, Flags...>& Status<o, Flags...>::reset_all()noexcept
{
    flags_.reset();
    return *this;
}

template <Object_Type::Enum o, typename ...Flags>
Status<o, Flags...>& Status<o, Flags...>::flip_all()noexcept
{
    flags_.flip();
    return *this;
}

template <Object_Type::Enum o, typename ...Flags>
Status<o, Flags...> Status<o, Flags...>::operator~()const noexcept
{
    return Status(~flags_);
}

template <Object_Type::Enum o, typename ...Flags>
constexpr Status<o, Flags...>::Status(const FlagsStorage& src)
    : flags_(src)
{
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//STATUS_IMPL_HH_7175EB6FC6ECBA7EE3E85161154E3603
