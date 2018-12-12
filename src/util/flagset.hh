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

#ifndef FLAGSET_HH_B8EC3B65AA1ABB0CDA7136BEAE08E971//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define FLAGSET_HH_B8EC3B65AA1ABB0CDA7136BEAE08E971

#include <bitset>

namespace Util {

enum class FlagSetVisiting
{
    can_continue,
    is_done
};

template <typename T, typename ...Flags>
class FlagSet
{
public:
    using Tag = T;
    FlagSet() = default;
    ~FlagSet() = default;
    FlagSet(const FlagSet&) = default;
    FlagSet(FlagSet&&) = default;
    FlagSet& operator=(const FlagSet&) = default;
    FlagSet& operator=(FlagSet&&) = default;

    static constexpr int number_of_flags = sizeof...(Flags);

    bool all()const noexcept;
    bool any()const noexcept;
    bool none()const noexcept;
    std::size_t count()const noexcept;
    constexpr std::size_t size()noexcept;

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
        return (flags_ & mask) != ItemsStorage();
    }

    template <typename ...Fs>
    bool are_unset_all_of()const noexcept
    {
        static const auto mask = mask_of<Fs...>();
        return (flags_ & mask) == ItemsStorage();
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
    FlagSet& set(bool value)noexcept
    {
        return value ? this->set<Fs...>()
                     : this->reset<Fs...>();
    }

    template <typename ...Fs>
    FlagSet& set()noexcept
    {
        static const auto mask = mask_of<Fs...>();
        flags_ |= mask;
        return *this;
    }

    template <typename ...Fs>
    FlagSet& reset()noexcept
    {
        static const auto mask = ~mask_of<Fs...>();
        flags_ &= mask;
        return *this;
    }

    template <typename ...Fs>
    FlagSet& flip()noexcept
    {
        static const auto mask = mask_of<Fs...>();
        flags_ ^= mask;
        return *this;
    }

    FlagSet& set_all(bool value)noexcept;
    FlagSet& set_all()noexcept;
    FlagSet& reset_all()noexcept;
    FlagSet& flip_all()noexcept;

    FlagSet operator~()const noexcept;

    FlagSet& operator&=(const FlagSet&)noexcept;
    FlagSet& operator|=(const FlagSet&)noexcept;
    FlagSet& operator^=(const FlagSet&)noexcept;

    /**
     * @tparam M a class with template method declared as
     *     template <typename F, int idx> FlagSetVisiting visit(FlagSet& fs);
     * @param mutator for each flag F with index idx is called mutator.visit<F, idx>(*this)
     *        iterating can be interrupted by returning FlagSetVisiting::is_done from the method visit
     */
    template <typename M>
    void visit(M&& mutator)
    {
        Iterate<0, Flags...>::over(std::forward<M>(mutator), *this);
    }

    /**
     * @tparam M a template class with template method declared as
     *     template <typename F, int idx> FlagSetVisiting visit(FlagSet& fs);
     * @tparam Ts types of parameters for constructing mutator of M<Flags...> type
     * @param args parameters for constructing mutator of M<Flags...> type; for each flag F with index idx
     *        is called mutator.visit<F, idx>(*this)
     *        iterating can be interrupted by returning FlagSetVisiting::is_done from the method visit
     * @return mutator used by iterating
     */
    template <template <typename...> class M, typename ...Ts>
    M<Flags...> visit(Ts&& ...args)
    {
        M<Flags...> mutator(std::forward<Ts>(args)...);
        Iterate<0, Flags...>::over(mutator, *this);
        return mutator;
    }

    /**
     * @tparam V a class with template method declared as
     *     template <typename F, int idx> FlagSetVisiting visit(const FlagSet& fs);
     * @param visitor for each flag F with index idx is called visitor.visit<F, idx>(*this)
     *        iterating can be interrupted by returning FlagSetVisiting::is_done from the method visit
     */
    template <typename V>
    void visit(V&& visitor)const
    {
        Iterate<0, Flags...>::over(std::forward<V>(visitor), *this);
    }

    /**
     * @tparam V a template class with template method declared as
     *     template <typename F, int idx> FlagSetVisiting visit(const FlagSet& fs);
     * @tparam Ts types of parameters for constructing visitor of V<Flags...> type
     * @param args parameters for constructing visitor of V<Flags...> type; for each flag F with index idx
     *        is called visitor.visit<F, idx>(*this)
     *        iterating can be interrupted by returning FlagSetVisiting::is_done from the method visit
     * @return visitor used by iterating
     */
    template <template <typename...> class V, typename ...Ts>
    V<Flags...> visit(Ts&& ...args)const
    {
        V<Flags...> visitor(std::forward<Ts>(args)...);
        Iterate<0, Flags...>::over(visitor, *this);
        return visitor;
    }

    /**
     * @tparam V a template class with template method declared as
     *     template <typename F, int idx> FlagSetVisiting visit();
     * @tparam Ts types of parameters for constructing visitor of V<Flags...> type
     * @param args parameters for constructing visitor of V<Flags...> type; for each flag F with index idx
     *        is called visitor.visit<F, idx>()
     *        iterating can be interrupted by returning FlagSetVisiting::is_done from the method visit
     * @return visitor used by iterating
     */
    template <template <typename...> class V, typename ...Ts>
    static V<Flags...> static_visit(Ts&& ...args)
    {
        V<Flags...> visitor(std::forward<Ts>(args)...);
        Iterate<0, Flags...>::over(visitor);
        return visitor;
    }
private:
    using ItemsStorage = std::bitset<sizeof...(Flags)>;
    constexpr FlagSet(const ItemsStorage& src);
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

    template <int idx, typename ...Fs>
    struct Iterate
    {
        static_assert(idx == sizeof...(Flags), "invalid usage, idx out of range");
        template <typename V>
        static void over(V&&, const FlagSet&) { }
        template <typename V>
        static void over(V&&) { }
    };
    template <int idx, typename F, typename ...Fs>
    struct Iterate<idx, F, Fs...>
    {
        template <typename V>
        static void over(V&& visitor, const FlagSet<Tag, Flags...>& status)
        {
            if (visitor.template visit<F, idx>(status) == FlagSetVisiting::can_continue)
            {
                Iterate<idx + 1, Fs...>::over(std::forward<V>(visitor), status);
            }
        }
        template <typename M>
        static void over(M&& mutator, FlagSet<Tag, Flags...>& status)
        {
            if (mutator.template visit<F, idx>(status) == FlagSetVisiting::can_continue)
            {
                Iterate<idx + 1, Fs...>::over(std::forward<M>(mutator), status);
            }
        }
        template <typename V>
        static void over(V&& visitor)
        {
            if (visitor.template visit<F, idx>() == FlagSetVisiting::can_continue)
            {
                Iterate<idx + 1, Fs...>::over(visitor);
            }
        }
    };

    template <typename F>
    static constexpr int index_of()noexcept
    {
        constexpr int flag_index = Get<F, Flags...>::distance_from_head();
        static_assert((0 <= flag_index) && (flag_index < sizeof...(Flags)), "index out of range");
        return flag_index;
    }

    static constexpr void set_bits(ItemsStorage&)noexcept { }
    template <typename ...Ts>
    static void set_bits(ItemsStorage& mask, int idx0, Ts ...idxs)noexcept
    {
        mask.set(idx0, true);
        set_bits(mask, idxs...);
    }

    template <typename ...Fs>
    static ItemsStorage mask_of()noexcept
    {
        ItemsStorage mask;
        set_bits(mask, index_of<Fs>()...);
        return mask;
    }

    template <typename ...Ts>
    struct IsUnique
    {
        static_assert(sizeof...(Ts) < 2, "used just for an absolutely sure unique type list");
        static constexpr bool result = true;
    };
    template <typename T0, typename T1, typename ...Ts>
    struct IsUnique<T0, T1, Ts...>
    {
        static constexpr bool result = IsUnique<T0, Ts...>::result &&
                                       IsUnique<T1, Ts...>::result;
    };
    template <typename T0, typename ...Ts>
    struct IsUnique<T0, T0, Ts...>
    {
        static constexpr bool result = false;
    };

    static_assert(IsUnique<Flags...>::result, "flag uniqueness violation");

    ItemsStorage flags_;
    friend bool operator==(const FlagSet& lhs, const FlagSet& rhs)noexcept
    {
        return lhs.flags_ == rhs.flags_;
    }
    friend bool operator!=(const FlagSet& lhs, const FlagSet& rhs)noexcept
    {
        return lhs.flags_ != rhs.flags_;
    }
    friend FlagSet operator&(const FlagSet& lhs, const FlagSet& rhs)noexcept
    {
        return FlagSet(lhs.flags_ & rhs.flags_);
    }
    friend FlagSet operator|(const FlagSet& lhs, const FlagSet& rhs)noexcept
    {
        return FlagSet(lhs.flags_ | rhs.flags_);
    }
    friend FlagSet operator^(const FlagSet& lhs, const FlagSet& rhs)noexcept
    {
        return FlagSet(lhs.flags_ ^ rhs.flags_);
    }
};

}//namespace Util

#endif//FLAGSET_HH_B8EC3B65AA1ABB0CDA7136BEAE08E971
