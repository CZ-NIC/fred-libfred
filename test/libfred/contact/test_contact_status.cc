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

#include "libfred/registrable_object/contact/contact_status.hh"
#include "libfred/registrable_object/contact/get_contact_status.hh"
#include "util/printable.hh"

#include <boost/test/unit_test.hpp>

#include <array>
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(TestContactStatus)

struct MyFixture
{
};

BOOST_FIXTURE_TEST_CASE(contact_status, MyFixture)
{
    using namespace LibFred::RegistrableObject::Contact;
    ContactStatus contact_status;
    contact_status.set<ConditionallyIdentifiedContact>();
    contact_status.set<ConditionallyIdentifiedContact, ServerBlocked, Linked>();
    BOOST_CHECK((contact_status.are_set_any_of<Linked,
                                               DeleteCandidate,
                                               ValidatedContact>()));
    BOOST_CHECK(contact_status.are_set_all_of<>());
    BOOST_CHECK(!contact_status.is_set<DeleteCandidate>());
    BOOST_CHECK(contact_status.is_set<Linked>());
//           contact_status.is_set<int>() ||
    BOOST_CHECK(!contact_status.is_set<ServerUpdateProhibited>());
    LibFred::OperationContextCreator ctx;
    const auto status1 = GetContactStatusById(1).exec(ctx);
    BOOST_CHECK(!status1.is_set<DeleteCandidate>());
    BOOST_CHECK(status1.is_set<Linked>());
    BOOST_CHECK(status1 == ContactStatus().set<Linked>());
    BOOST_CHECK(~status1 == ~ContactStatus().set<Linked>());
    BOOST_CHECK(~status1 != ContactStatus().set<Linked>());
    BOOST_CHECK(status1 != ~ContactStatus().set<Linked>());
}

BOOST_AUTO_TEST_SUITE_END()//TestContactStatus
