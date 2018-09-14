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

#ifndef STATUS_HH_D50CE7E35AB8802662C58685DEC83AD8//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STATUS_HH_D50CE7E35AB8802662C58685DEC83AD8

#include "libfred/object/object_type.hh"

#include <bitset>

namespace LibFred {
namespace RegistrableObject {

struct StatusFlag
{
    enum class Manipulation
    {
        manual,
        automatic
    };
    enum class Visibility
    {
        external,
        internal
    };
    template <Manipulation m, Visibility v, typename>
    struct Type
    {
        static constexpr Manipulation how_to_set = m;
        static constexpr Visibility visibility = v;
    };
    template <typename N>
    using AutomaticExternal = Type<Manipulation::automatic, Visibility::external, N>;
    template <typename N>
    using AutomaticInternal = Type<Manipulation::automatic, Visibility::internal, N>;
    template <typename N>
    using ManualExternal = Type<Manipulation::manual, Visibility::external, N>;
    template <typename N>
    using ManualInternal = Type<Manipulation::manual, Visibility::internal, N>;
};

template <Object_Type::Enum o, typename ...Flags>
class Status
{
public:
    Status() = default;
    ~Status() = default;
    Status(const Status&) = default;
    Status(Status&&) = default;
    Status& operator=(const Status&) = default;
    Status& operator=(Status&&) = default;

    static constexpr Object_Type::Enum object_type = o;
    static constexpr int number_of_flags = sizeof...(Flags);

    bool all()const noexcept
    {
        return flags_.all();
    }
    bool any()const noexcept
    {
        return flags_.any();
    }
    bool none()const noexcept
    {
        return flags_.none();
    }
    std::size_t count()const noexcept
    {
        return flags_.count();
    }
    constexpr std::size_t size()noexcept
    {
        return FlagsStorage().size();
    }

    template <typename ...Fs>
    bool are_set_all_of()const noexcept
    {
        static const auto mask = mask_of<Fs...>();
        return (flags_ & mask) == mask;
    }
    template <typename ...Fs>
    bool are_set_any_of()const noexcept
    {
        static const auto mask = mask_of<Fs...>();
        return (flags_ & mask) != FlagsStorage();
    }

    template <typename ...Fs>
    bool are_unset_all_of()const noexcept
    {
        static const auto mask = mask_of<Fs...>();
        return (flags_ & mask) == FlagsStorage();
    }
    template <typename ...Fs>
    bool are_unset_any_of()const noexcept
    {
        static const auto mask = mask_of<Fs...>();
        return (flags_ & mask) != mask;
    }

    template <typename F>
    bool is_set()const noexcept
    {
        return flags_.test(index_of<F>());
    }

    template <typename ...Fs>
    Status& set(bool value)noexcept
    {
        return value ? this->set<Fs...>()
                     : this->reset<Fs...>();
    }
    template <typename ...Fs>
    Status& set()noexcept
    {
        flags_ |= mask_of<Fs...>();
        return *this;
    }
    template <typename ...Fs>
    Status& reset()noexcept
    {
        flags_ &= ~mask_of<Fs...>();
        return *this;
    }
    template <typename ...Fs>
    Status& flip()noexcept
    {
        flags_ ^= mask_of<Fs...>();
        return *this;
    }

    Status& set_all(bool value)noexcept
    {
        if (value)
        {
            return this->set_all();
        }
        return this->reset_all();
    }
    Status& set_all()noexcept
    {
        flags_.set();
        return *this;
    }
    Status& reset_all()noexcept
    {
        flags_.reset();
        return *this;
    }
    Status& flip_all()noexcept
    {
        flags_.flip();
        return *this;
    }

    Status operator~()const noexcept
    {
        return Status(~flags_);
    }

    template <typename F>
    static constexpr int get_index_of()noexcept
    {
        return index_of<F>();
    }
    template <template <typename ...> class T>
    struct FlagsIn
    {
        using Type = T<Flags...>;
    };
private:
    using FlagsStorage = std::bitset<sizeof...(Flags)>;
    constexpr Status(const FlagsStorage& src)
        : flags_(src)
    {
    }
    template <typename I, typename H, typename ...Ts>
    struct Get
    {
        static constexpr int distance_from_head()
        {
            static_assert(0 < sizeof...(Ts), "flag not found in set of available flags");
            constexpr int result = Get<I, Ts...>::distance_from_head() + 1;
            return result;
        }
    };
    template <typename I, typename ...Ts>
    struct Get<I, I, Ts...>
    {
        static constexpr int distance_from_head()
        {
            return 0;
        }
    };
    template <typename F>
    static constexpr int index_of()noexcept
    {
        constexpr int flag_index = Get<F, Flags...>::distance_from_head();
        static_assert((0 <= flag_index) && (flag_index < sizeof...(Flags)), "index out of range");
        return flag_index;
    }
    static constexpr void set_bits(FlagsStorage&)noexcept
    {
    }
    template <typename ...Ts>
    static void set_bits(FlagsStorage& mask, int idx0, Ts ...idxs)noexcept
    {
        mask.set(idx0, true);
        set_bits(mask, idxs...);
    }
    template <typename ...Fs>
    static FlagsStorage mask_of()noexcept
    {
        FlagsStorage mask;
        set_bits(mask, index_of<Fs>()...);
        return mask;
    }
    FlagsStorage flags_;
    friend bool operator==(const Status& lhs, const Status& rhs)noexcept
    {
        return lhs.flags_ == rhs.flags_;
    }
    friend bool operator!=(const Status& lhs, const Status& rhs)noexcept
    {
        return lhs.flags_ != rhs.flags_;
    }
    friend Status operator&(const Status& lhs, const Status& rhs)noexcept
    {
        return Status(lhs.flags_ & rhs.flags_);
    }
    friend Status operator|(const Status& lhs, const Status& rhs)noexcept
    {
        return Status(lhs.flags_ | rhs.flags_);
    }
    friend Status operator^(const Status& lhs, const Status& rhs)noexcept
    {
        return Status(lhs.flags_ ^ rhs.flags_);
    }
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//STATUS_HH_D50CE7E35AB8802662C58685DEC83AD8
