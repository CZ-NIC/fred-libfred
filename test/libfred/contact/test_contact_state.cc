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

#include "libfred/registrable_object/contact/contact_state.hh"
#include "libfred/registrable_object/contact/get_contact_state.hh"
#include "libfred/registrable_object/contact/get_contact_state_history.hh"
#include "util/printable.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <array>
#include <sstream>
#include <string>

BOOST_AUTO_TEST_SUITE(TestContactState)

using TimePointConverter = SqlConvert<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>>;

struct SimpleStateView
{
    template <typename ...Fs>
    struct Visitor
    {
        using State = LibFred::RegistrableObject::State;
        template <typename F, int, typename Tag>
        Util::FlagSetVisiting visit(
                const Util::FlagSet<Tag, Fs...>& state)
        {
            result.append(state.template is_set<F>() ? "*" : ".");
            return Util::FlagSetVisiting::can_continue;
        }
        std::string result;
    };
};

template <typename Tag, typename ...Fs>
std::string simple_state_view(const Util::FlagSet<Tag, Fs...>& state)
{
    return state.template visit<SimpleStateView::template Visitor>().result;
}

BOOST_FIXTURE_TEST_CASE(contact_state, Test::instantiate_db_template)
{
    using namespace LibFred::RegistrableObject::Contact;
    ContactState contact_state;
    contact_state.set<ConditionallyIdentifiedContact>();
    contact_state.set<ConditionallyIdentifiedContact, ServerBlocked, Linked>();
    BOOST_CHECK((contact_state.are_set_any_of<Linked,
                                              DeleteCandidate,
                                              ValidatedContact>()));
    BOOST_CHECK(contact_state.are_set_all_of<>());
    BOOST_CHECK(!contact_state.is_set<DeleteCandidate>());
    BOOST_CHECK(contact_state.is_set<Linked>());
//           contact_state.is_set<int>() ||
    BOOST_CHECK(!contact_state.is_set<ServerUpdateProhibited>());
    LibFred::OperationContextCreator ctx;
    const auto state1 = GetContactStateById(1).exec(ctx);
    BOOST_CHECK(!state1.is_set<DeleteCandidate>());
    BOOST_CHECK(state1.is_set<Linked>());
    BOOST_CHECK(state1 == ContactState().set<Linked>());
    BOOST_CHECK(~state1 == ~ContactState().set<Linked>());
    BOOST_CHECK(~state1 != ContactState().set<Linked>());
    BOOST_CHECK(state1 != ~ContactState().set<Linked>());
    const auto interval = LibFred::RegistrableObject::HistoryInterval(
            LibFred::RegistrableObject::HistoryInterval::LowerLimit(
//                            TimePointConverter::from("2018-08-29 13:42:41.478909")),
                    LibFred::RegistrableObject::HistoryInterval::NoLimit()),
            LibFred::RegistrableObject::HistoryInterval::UpperLimit(
//                            TimePointConverter::from("2018-08-29 13:42:41.907719"))));
                    LibFred::RegistrableObject::HistoryInterval::NoLimit()));

    const auto state1_history = GetContactStateHistoryById(1).exec(ctx, interval);
    std::ostringstream out;
    for (const auto& record : state1_history.timeline)
    {
        out << TimePointConverter::to(record.valid_from) << " - "
            << simple_state_view(record.state) << std::endl;
    }
    out << TimePointConverter::to(state1_history.valid_to) << std::endl;
    GetContactStateByUuid(1).exec(ctx);
    GetContactStateByUuid("1").exec(ctx);
    GetContactStateByUuid("KONTAKT").exec(ctx);
    GetContactStateHistoryByUuid(1).exec(ctx, interval);
    GetContactStateHistoryByUuid("1").exec(ctx, interval);
    GetContactStateHistoryByUuid("KONTAKT").exec(ctx, interval);
}

BOOST_AUTO_TEST_SUITE_END()//TestContactState
