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
#ifndef FLAGSET_IMPL_HH_9762E715E57BB0911C9A1C4301C215BE//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define FLAGSET_IMPL_HH_9762E715E57BB0911C9A1C4301C215BE

#include "util/flagset.hh"

namespace Util {

template <typename Tag, typename ...Flags>
bool FlagSet<Tag, Flags...>::all()const noexcept
{
    return flags_.all();
}

template <typename Tag, typename ...Flags>
bool FlagSet<Tag, Flags...>::any()const noexcept
{
    return flags_.any();
}

template <typename Tag, typename ...Flags>
bool FlagSet<Tag, Flags...>::none()const noexcept
{
    return flags_.none();
}

template <typename Tag, typename ...Flags>
std::size_t FlagSet<Tag, Flags...>::count()const noexcept
{
    return flags_.count();
}

template <typename Tag, typename ...Flags>
constexpr std::size_t FlagSet<Tag, Flags...>::size()noexcept
{
    return ItemsStorage().size();
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::set_all(bool value)noexcept
{
    if (value)
    {
        return this->set_all();
    }
    return this->reset_all();
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::set_all()noexcept
{
    flags_.set();
    return *this;
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::reset_all()noexcept
{
    flags_.reset();
    return *this;
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::flip_all()noexcept
{
    flags_.flip();
    return *this;
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...> FlagSet<Tag, Flags...>::operator~()const noexcept
{
    return FlagSet<Tag, Flags...>(~flags_);
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::operator&=(const FlagSet& src)noexcept
{
    flags_ &= src.flags_;
    return *this;
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::operator|=(const FlagSet& src)noexcept
{
    flags_ |= src.flags_;
    return *this;
}

template <typename Tag, typename ...Flags>
FlagSet<Tag, Flags...>& FlagSet<Tag, Flags...>::operator^=(const FlagSet& src)noexcept
{
    flags_ ^= src.flags_;
    return *this;
}

template <typename Tag, typename ...Flags>
constexpr FlagSet<Tag, Flags...>::FlagSet(const ItemsStorage& src)
    : flags_(src)
{
}

template <typename Tag, typename ...Flags>
constexpr int FlagSet<Tag, Flags...>::number_of_flags;

}//namespace Util

#endif//FLAGSET_IMPL_HH_9762E715E57BB0911C9A1C4301C215BE
