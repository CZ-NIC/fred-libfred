/*
 * Copyright (C) 2013  CZ.NIC, z.s.p.o.
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
#include "util/printable.hh"

#include <boost/test/unit_test.hpp>

#include <array>
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(TestContactStatus)

struct MyFixture
{
};

template <typename>
struct ToString;

template <>
struct ToString<LibFred::RegistrableObject::Contact::DeleteCandidate>
{
    static const char* value() { return "deleteCandidate"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::Linked>
{
    static const char* value() { return "linked"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ContactFailedManualVerification>
{
    static const char* value() { return "contactFailedManualVerification"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ContactInManualVerification>
{
    static const char* value() { return "contactInManualVerification"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ContactPassedManualVerification>
{
    static const char* value() { return "contactPassedManualVerification"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ConditionallyIdentifiedContact>
{
    static const char* value() { return "conditionallyIdentifiedContact"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::IdentifiedContact>
{
    static const char* value() { return "identifiedContact"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ValidatedContact>
{
    static const char* value() { return "validatedContact"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::MojeidContact>
{
    static const char* value() { return "mojeidContact"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ServerBlocked>
{
    static const char* value() { return "serverBlocked"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ServerDeleteProhibited>
{
    static const char* value() { return "serverDeleteProhibited"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ServerTransferProhibited>
{
    static const char* value() { return "serverTransferProhibited"; }
};

template <>
struct ToString<LibFred::RegistrableObject::Contact::ServerUpdateProhibited>
{
    static const char* value() { return "serverUpdateProhibited"; }
};

struct StatusFlagInfo : Util::Printable<StatusFlagInfo>
{
    template <typename F>
    static StatusFlagInfo make_from()
    {
        StatusFlagInfo result;
        result.name = ToString<F>::value();
        result.how_to_set = F::how_to_set;
        result.visibility = F::visibility;
        return result;
    }
    std::string to_string()const
    {
        std::string result = name;
        switch (how_to_set)
        {
            case LibFred::RegistrableObject::StatusFlag::Manipulation::automatic:
                result += " automatic";
                break;
            case LibFred::RegistrableObject::StatusFlag::Manipulation::manual:
                result += " manual";
                break;
        }
        switch (visibility)
        {
            case LibFred::RegistrableObject::StatusFlag::Visibility::external:
                result += " external";
                break;
            case LibFred::RegistrableObject::StatusFlag::Visibility::internal:
                result += " internal";
                break;
        }
        return result;
    }
    std::string name;
    LibFred::RegistrableObject::StatusFlag::Manipulation how_to_set;
    LibFred::RegistrableObject::StatusFlag::Visibility visibility;
};

template <typename ...Flags>
struct TemplateCollectStatusFlagsInfo
{
    using Result = std::array<StatusFlagInfo, sizeof...(Flags)>;
    static Result run()
    {
        Result result;
        List<Flags...>::run(result.data());
        return result;
    }
    template <typename ...Fs> struct List
    {
        static void run(const StatusFlagInfo*) { }
    };
    template <typename F, typename ...Fs>
    struct List<F, Fs...>
    {
        static void run(StatusFlagInfo *dst)
        {
            *dst = StatusFlagInfo::make_from<F>();
            List<Fs...>::run(dst + 1);
        }
    };
};

BOOST_FIXTURE_TEST_CASE(contact_status, MyFixture)
{
    using namespace LibFred::RegistrableObject::Contact;
    using CollectStatusFlagsInfo = typename ContactStatus::FlagsIn<TemplateCollectStatusFlagsInfo>::Type;
    const auto info = CollectStatusFlagsInfo::run();
    for (const auto& flag : info)
    {
        std::cout << flag << std::endl;
    }
    ContactStatus contact_status;
    BOOST_CHECK_EQUAL(ContactStatus::get_index_of<DeleteCandidate>(), 0);
    BOOST_CHECK_EQUAL(ContactStatus::get_index_of<Linked>(), 1);
    BOOST_CHECK_EQUAL(ContactStatus::get_index_of<ServerUpdateProhibited>(), 12);
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
}

BOOST_AUTO_TEST_SUITE_END()//TestContactStatus
