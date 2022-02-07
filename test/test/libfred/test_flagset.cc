/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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
#include "util/flagset.hh"
#include "util/flagset_impl.hh"

#include <boost/optional.hpp>
#include <boost/test/unit_test.hpp>

#include <iosfwd>
#include <set>
#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(TestFlagSet)

namespace {

template <const char* name>
struct Flag
{
    static std::string to_string() { return name; }
};

namespace StateFlagName {

extern const char a[] = "flag A";
extern const char b[] = "flag B";
extern const char c[] = "flag C";
extern const char d[] = "flag D";
extern const char e[] = "flag E";

}//namespace {anonymous}::StateFlagName

struct MyFixture
{
    using FlagA = Flag<StateFlagName::a>;
    using FlagB = Flag<StateFlagName::b>;
    using FlagC = Flag<StateFlagName::c>;
    using FlagD = Flag<StateFlagName::d>;
    using FlagE = Flag<StateFlagName::e>;
    using FlagSet1 = Util::FlagSet<struct One, FlagA>;
    using FlagSet2 = Util::FlagSet<struct Two, FlagA, FlagB>;
    using FlagSet3 = Util::FlagSet<struct Three, FlagA, FlagB, FlagC>;
    using FlagSet4 = Util::FlagSet<struct Four, FlagA, FlagB, FlagC, FlagD>;
    using FlagSet5 = Util::FlagSet<struct Five, FlagA, FlagB, FlagC, FlagD, FlagE>;
};

template <typename S>
auto emplace_into(S&& s)
{
    return std::forward<S>(s);
}

template <typename S, typename T, typename ...Ts>
auto emplace_into(S&& s, T&& arg, Ts&& ...args)
{
    s.emplace(std::forward<T>(arg));
    return std::forward<decltype(emplace_into(std::forward<S>(s), std::forward<Ts>(args)...))>(
            emplace_into(std::forward<S>(s), std::forward<Ts>(args)...));
}

//used as M in void visit(M&& mutator)
struct SetFlagsByName
{
    template <typename ...Ts>
    SetFlagsByName(Ts&& ...args)
        : to_set(emplace_into(std::set<std::string>(), std::forward<Ts>(args)...)) { }
    template <typename F, int idx, typename N, typename ...Fs>
    Util::FlagSetVisiting visit(Util::FlagSet<N, Fs...>& fs)const
    {
        const bool flag_value = 0 < to_set.count(F::to_string());
        fs.template set<F>(flag_value);
        return Util::FlagSetVisiting::can_continue;
    }
    const std::set<std::string> to_set;
};

//used as M in M<Flags...> visit(Ts&& ...args)
template <typename ...Fs>
struct SetFlagsByNameAndStoreItsIdx
{
    template <typename ...Ts>
    SetFlagsByNameAndStoreItsIdx(Ts&& ...args)
        : to_set(emplace_into(std::set<std::string>(), std::forward<Ts>(args)...)) { }
    template <typename F, int idx, typename N>
    Util::FlagSetVisiting visit(Util::FlagSet<N, Fs...>& fs)
    {
        const bool flag_value = 0 < to_set.count(F::to_string());
        fs.template set<F>(flag_value);
        if (flag_value)
        {
            flag_idx.insert(idx);
        }
        return Util::FlagSetVisiting::can_continue;
    }
    std::set<std::string> to_set;
    std::set<int> flag_idx;
};

//used as V in void visit(V&& visitor)const
struct GetIdxOfFirstFlag
{
    template <typename ...Ts>
    GetIdxOfFirstFlag(Ts&& ...args)
        : flags_to_test(emplace_into(std::set<std::string>(), std::forward<Ts>(args)...))
    {
        result = boost::none;
    }
    template <typename F, int idx, typename N, typename ...Fs>
    Util::FlagSetVisiting visit(const Util::FlagSet<N, Fs...>& fs)
    {
        const bool flag_has_to_be_test = 0 < flags_to_test.count(F::to_string());
        if (flag_has_to_be_test &&
            fs.template is_set<F>())
        {
            result = idx;
            return Util::FlagSetVisiting::is_done;
        }
        return Util::FlagSetVisiting::can_continue;
    }
    std::set<std::string> flags_to_test;
    boost::optional<int> result;
};

//used as V in V<Flags...> visit(Ts&& ...args)const
template <typename ...Fs>
struct IsSetAnyOf
{
    template <typename ...Ts>
    IsSetAnyOf(Ts&& ...args)
        : flags_to_test(emplace_into(std::set<std::string>(), std::forward<Ts>(args)...)),
          result(false)
    { }
    template <typename F, int idx, typename N>
    Util::FlagSetVisiting visit(const Util::FlagSet<N, Fs...>& fs)
    {
        const bool flag_has_to_be_test = 0 < flags_to_test.count(F::to_string());
        if (flag_has_to_be_test && fs.template is_set<F>())
        {
            result = true;
            return Util::FlagSetVisiting::is_done;
        }
        return Util::FlagSetVisiting::can_continue;
    }
    const std::set<std::string> flags_to_test;
    bool result;
};

template <typename N, const char* ...names>
std::ostream& operator<<(std::ostream&, const Util::FlagSet<N, Flag<names>...>&);

}//namespace {anonymous}

BOOST_FIXTURE_TEST_CASE(flagset1, MyFixture)
{
    using FlagSet = FlagSet1;
    FlagSet fs;
    BOOST_CHECK_EQUAL(FlagSet::number_of_flags, 1);
    BOOST_CHECK(!fs.all());
    BOOST_CHECK(!fs.any());
    BOOST_CHECK(fs.none());
    BOOST_CHECK_EQUAL(fs.count(), 0);
    BOOST_CHECK_EQUAL(fs.size(), FlagSet::number_of_flags);
    BOOST_CHECK(!fs.are_set_all_of<FlagA>());
    BOOST_CHECK(!fs.are_set_any_of<FlagA>());
    BOOST_CHECK(fs.are_unset_all_of<FlagA>());
    BOOST_CHECK(fs.are_unset_any_of<FlagA>());
    BOOST_CHECK(!fs.is_set<FlagA>());
    BOOST_CHECK(fs == FlagSet().reset_all());
    BOOST_CHECK(FlagSet().reset_all() == fs);
    BOOST_CHECK(fs != FlagSet().set_all());
    BOOST_CHECK(FlagSet().set_all() != fs);
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());
    BOOST_CHECK_EQUAL(FlagSet().reset_all(), fs);
    BOOST_CHECK_NE(fs, FlagSet().set_all());
    BOOST_CHECK_NE(FlagSet().set_all(), fs);

    fs.set<FlagA>();
    BOOST_CHECK(fs.all());
    BOOST_CHECK(fs.any());
    BOOST_CHECK(!fs.none());
    BOOST_CHECK_EQUAL(fs.count(), 1);
    BOOST_CHECK(fs.are_set_all_of<FlagA>());
    BOOST_CHECK(fs.are_set_any_of<FlagA>());
    BOOST_CHECK(!fs.are_unset_all_of<FlagA>());
    BOOST_CHECK(!fs.are_unset_any_of<FlagA>());
    BOOST_CHECK(fs.is_set<FlagA>());
    BOOST_CHECK(fs == FlagSet().set_all());
    BOOST_CHECK(FlagSet().set_all() == fs);
    BOOST_CHECK(fs != FlagSet().reset_all());
    BOOST_CHECK(FlagSet().reset_all() != fs);
    BOOST_CHECK_EQUAL(fs, FlagSet().set_all());
    BOOST_CHECK_EQUAL(FlagSet().set_all(), fs);
    BOOST_CHECK_NE(fs, FlagSet().reset_all());
    BOOST_CHECK_NE(FlagSet().reset_all(), fs);

    fs.set<FlagA>(false);
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());

    fs.set<FlagA>(true);
    BOOST_CHECK_EQUAL(fs, FlagSet().set_all());

    fs.flip_all();
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());

    fs.flip_all();
    BOOST_CHECK_EQUAL(fs, FlagSet().set_all());

    fs.flip<FlagA>();
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());

    fs.flip<FlagA>();
    BOOST_CHECK_EQUAL(fs, FlagSet().set_all());

    BOOST_CHECK_EQUAL(~FlagSet(), FlagSet().set_all());
    BOOST_CHECK_EQUAL(FlagSet(), ~(FlagSet().set_all()));

    fs = FlagSet();
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());
    fs = FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, FlagSet().set_all());

    fs = FlagSet();
    fs &= FlagSet();
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs &= fs;
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs &= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, FlagSet());

    fs = FlagSet().set_all();
    fs &= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs &= fs;
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs &= FlagSet();
    BOOST_CHECK_EQUAL(fs, FlagSet());

    fs = FlagSet();
    fs |= FlagSet();
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs |= fs;
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs |= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());

    fs = FlagSet().set_all();
    fs |= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs |= fs;
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs |= FlagSet();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());

    fs = FlagSet();
    fs ^= FlagSet();
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs ^= fs;
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs ^= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());

    fs ^= FlagSet().set_all();
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs = FlagSet().set_all();
    fs ^= fs;
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs = FlagSet().set_all();
    fs ^= FlagSet();
    BOOST_CHECK_EQUAL(fs, ~FlagSet());

    BOOST_CHECK_EQUAL(FlagSet() & FlagSet(), FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() & FlagSet(), FlagSet());
    BOOST_CHECK_EQUAL(FlagSet() & ~FlagSet(), FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() & ~FlagSet(), ~FlagSet());

    BOOST_CHECK_EQUAL(FlagSet() | FlagSet(), FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() | FlagSet(), ~FlagSet());
    BOOST_CHECK_EQUAL(FlagSet() | ~FlagSet(), ~FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() | ~FlagSet(), ~FlagSet());

    BOOST_CHECK_EQUAL(FlagSet() ^ FlagSet(), FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() ^ FlagSet(), ~FlagSet());
    BOOST_CHECK_EQUAL(FlagSet() ^ ~FlagSet(), ~FlagSet());
    BOOST_CHECK_EQUAL(~FlagSet() ^ ~FlagSet(), FlagSet());

    //void visit(M&& mutator)
    fs.visit(SetFlagsByName());
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs.visit(SetFlagsByName("xxx"));
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs.visit(SetFlagsByName(FlagB::to_string()));
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs.visit(SetFlagsByName(FlagB::to_string(), StateFlagName::c));
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs.visit(SetFlagsByName(FlagB::to_string(), StateFlagName::c, FlagA::to_string()));
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs.visit(SetFlagsByName(FlagB::to_string(), StateFlagName::a));
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs.visit(SetFlagsByName(FlagA::to_string()));
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    fs.visit(SetFlagsByName("flag A"));
    BOOST_CHECK_EQUAL(fs, ~FlagSet());

    //M<Flags...> visit(Ts&& ...args)
    auto mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>();
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 0);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>("xxx");
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 0);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>(FlagB::to_string());
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 0);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>(FlagB::to_string(), StateFlagName::c);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 0);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>(FlagB::to_string(), StateFlagName::c, FlagA::to_string());
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 1);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>(FlagB::to_string(), StateFlagName::a);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 1);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>(FlagA::to_string());
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 1);
    mutator = fs.visit<SetFlagsByNameAndStoreItsIdx>("flag A");
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(mutator.flag_idx.size(), 1);

    //void visit(V&& visitor)const
    const auto& cfs = fs;
    GetIdxOfFirstFlag get_idx_of_first_flag;
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);

    get_idx_of_first_flag = GetIdxOfFirstFlag(FlagB::to_string());
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);

    get_idx_of_first_flag = GetIdxOfFirstFlag("");
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);

    get_idx_of_first_flag = GetIdxOfFirstFlag("xxx");
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);

    get_idx_of_first_flag = GetIdxOfFirstFlag(FlagA::to_string());
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);

    get_idx_of_first_flag = GetIdxOfFirstFlag(StateFlagName::a);
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);

    get_idx_of_first_flag = GetIdxOfFirstFlag(FlagA::to_string(), FlagB::to_string());
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);

    get_idx_of_first_flag = GetIdxOfFirstFlag(StateFlagName::a, FlagB::to_string());
    fs = FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(get_idx_of_first_flag.result == boost::none);
    fs = ~FlagSet();
    cfs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);
    fs.visit(get_idx_of_first_flag);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK_EQUAL(*get_idx_of_first_flag.result, 0);

    //V<Flags...> visit(Ts&& ...args)const
    fs = FlagSet();
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>().result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>("").result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>("xxx").result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagB::to_string()).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::c).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::a).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagA::to_string()).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>().result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>("").result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>("xxx").result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagB::to_string()).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::c).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::a).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagA::to_string()).result);
    BOOST_CHECK_EQUAL(fs, FlagSet());
    fs = ~FlagSet();
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>().result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>("").result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>("xxx").result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagB::to_string()).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!cfs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::c).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(cfs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::a).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(cfs.visit<IsSetAnyOf>(FlagA::to_string()).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>().result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>("").result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>("xxx").result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagB::to_string()).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(!fs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::c).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(fs.visit<IsSetAnyOf>(FlagB::to_string(), StateFlagName::a).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
    BOOST_CHECK(fs.visit<IsSetAnyOf>(FlagA::to_string()).result);
    BOOST_CHECK_EQUAL(fs, ~FlagSet());
}

BOOST_FIXTURE_TEST_CASE(flagset5, MyFixture)
{
    using FlagSet = FlagSet5;
    FlagSet fs;
    BOOST_CHECK_EQUAL(FlagSet::number_of_flags, 5);
    BOOST_CHECK(!fs.all());
    BOOST_CHECK(!fs.any());
    BOOST_CHECK(fs.none());
    BOOST_CHECK_EQUAL(fs.count(), 0);
    BOOST_CHECK_EQUAL(fs.size(), FlagSet::number_of_flags);
    BOOST_CHECK(!fs.are_set_all_of<FlagA>());
    BOOST_CHECK(!fs.are_set_any_of<FlagA>());
    BOOST_CHECK(fs.are_unset_all_of<FlagA>());
    BOOST_CHECK(fs.are_unset_any_of<FlagA>());
    BOOST_CHECK(!fs.is_set<FlagA>());
    BOOST_CHECK(fs == FlagSet().reset_all());
    BOOST_CHECK(FlagSet().reset_all() == fs);
    BOOST_CHECK(fs != FlagSet().set_all());
    BOOST_CHECK(FlagSet().set_all() != fs);
    BOOST_CHECK_EQUAL(fs, FlagSet().reset_all());
    BOOST_CHECK_EQUAL(FlagSet().reset_all(), fs);
    BOOST_CHECK_NE(fs, FlagSet().set_all());
    BOOST_CHECK_NE(FlagSet().set_all(), fs);

    fs.set<FlagA>();
    BOOST_CHECK(!fs.all());
    BOOST_CHECK(fs.any());
    BOOST_CHECK(!fs.none());
    BOOST_CHECK_EQUAL(fs.count(), 1);
    BOOST_CHECK(fs.are_set_all_of<FlagA>());
    BOOST_CHECK(!fs.are_set_all_of<FlagB>());
    BOOST_CHECK(!fs.are_set_all_of<FlagC>());
    BOOST_CHECK(!fs.are_set_all_of<FlagD>());
    BOOST_CHECK(!fs.are_set_all_of<FlagE>());
    BOOST_CHECK(!(fs.are_set_all_of<FlagA, FlagB>()));
    BOOST_CHECK(!(fs.are_set_all_of<FlagA, FlagB, FlagC>()));
    BOOST_CHECK(!(fs.are_set_all_of<FlagA, FlagB, FlagC, FlagD>()));
    BOOST_CHECK(!(fs.are_set_all_of<FlagA, FlagB, FlagC, FlagD, FlagE>()));
    BOOST_CHECK(!(fs.are_set_all_of<FlagE, FlagD, FlagC, FlagB, FlagA>()));
    BOOST_CHECK(fs.are_set_any_of<FlagA>());
    BOOST_CHECK(!fs.are_set_any_of<FlagB>());
    BOOST_CHECK(!fs.are_set_any_of<FlagC>());
    BOOST_CHECK(!fs.are_set_any_of<FlagD>());
    BOOST_CHECK(!fs.are_set_any_of<FlagE>());
    BOOST_CHECK((fs.are_set_any_of<FlagA, FlagB>()));
    BOOST_CHECK((fs.are_set_any_of<FlagA, FlagB, FlagC>()));
    BOOST_CHECK((fs.are_set_any_of<FlagA, FlagB, FlagC, FlagD>()));
    BOOST_CHECK((fs.are_set_any_of<FlagA, FlagB, FlagC, FlagD, FlagE>()));
    BOOST_CHECK((fs.are_set_any_of<FlagE, FlagD, FlagC, FlagB, FlagA>()));
    BOOST_CHECK(!fs.are_unset_all_of<FlagA>());
    BOOST_CHECK((fs.are_unset_all_of<FlagB, FlagC, FlagD, FlagE>()));
    BOOST_CHECK(!fs.are_unset_any_of<FlagA>());
    BOOST_CHECK((fs.are_unset_any_of<FlagA, FlagB, FlagC, FlagD, FlagE>()));
    BOOST_CHECK(fs.is_set<FlagA>());
    BOOST_CHECK(fs != FlagSet().set_all());
    BOOST_CHECK(FlagSet().set_all() != fs);
    BOOST_CHECK(fs != FlagSet().reset_all());
    BOOST_CHECK(FlagSet().reset_all() != fs);
    BOOST_CHECK_NE(fs, FlagSet().set_all());
    BOOST_CHECK_NE(FlagSet().set_all(), fs);
    BOOST_CHECK_NE(fs, FlagSet().reset_all());
    BOOST_CHECK_NE(FlagSet().reset_all(), fs);
}

namespace {

//used as V in V<Flags...> static_visit(Ts&& ...args)
template <typename ...Fs>
struct GetFlagNames
{
    template <typename F, int idx>
    Util::FlagSetVisiting visit()
    {
        if (!result.empty())
        {
            result += ", ";
        }
        result += std::to_string(idx) + ":\"" + F::to_string() + "\"";
        return Util::FlagSetVisiting::can_continue;
    }
    std::string result;
};

//used as V in V<Flags...> visit(Ts&& ...args)const
template <typename ...Fs>
struct GetFlagValues
{
    template <typename F, int, typename N>
    Util::FlagSetVisiting visit(const Util::FlagSet<N, Fs...>& flags)
    {
        if (flags.template is_set<F>())
        {
            result += "*";
        }
        else
        {
            result += "-";
        }
        return Util::FlagSetVisiting::can_continue;
    }
    std::string result;
};

template <typename N, const char* ...names>
std::ostream& operator<<(std::ostream& out, const Util::FlagSet<N, Flag<names>...>& fs)
{
    using FlagSet = Util::FlagSet<N, Flag<names>...>;
    return out << "[" << FlagSet::template static_visit<GetFlagNames>().result << "]."
               << fs.template visit<GetFlagValues>().result;
}

}//namespace {anonymous}

BOOST_AUTO_TEST_SUITE_END()//TestFlagSet
