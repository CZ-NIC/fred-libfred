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
/**
 *  @file
 */

#include <boost/test/unit_test.hpp>
#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "test/libfred/notifier/util.hh"

#include "libfred/notifier/gather_email_data/gather_email_addresses.hh"


BOOST_AUTO_TEST_SUITE(TestNotifier)
BOOST_AUTO_TEST_SUITE(GatherEmailAddresses)
BOOST_AUTO_TEST_SUITE(Nsset)
BOOST_AUTO_TEST_SUITE(Create)

/* time: -inf ---> history ---> notified_version ---> future ---> +inf */
const unsigned history_starts_years_ago = 4;
const unsigned notified_version_starts_years_ago = 3;
const unsigned notification_is_years_ago = 2;
const unsigned future_starts_years_ago = 1;

struct has_nsset : has_autocomitting_ctx {
    const ::LibFred::InfoRegistrarData registrar;

    ::LibFred::InfoContactData nsset_tech_c1_to_be_notified;
    ::LibFred::InfoContactData nsset_tech_c2_to_be_notified;
    ::LibFred::InfoNssetData nsset;
    has_nsset()
    :
        registrar(Test::registrar(ctx).info_data)
    {
        const std::string tech_c1_handle = "NSSET1_TECH_C1";
        {
            const unsigned long long crhid =
                Test::exec(
                    ::LibFred::CreateContact(tech_c1_handle, registrar.handle)
                        .set_email("history.nsset1.tech.c.1@.nic.cz")
                        .set_notifyemail("history.nsset1.tech.c.1.notify@.nic.cz"),
                    ctx
                ).historyid;
            const unsigned long long to_be_notified_hid =
                ::LibFred::UpdateContactByHandle(tech_c1_handle, registrar.handle)
                    .set_email("nsset1.tech.c.1@.nic.cz")
                    .set_notifyemail("nsset1.tech.c.1.notify@.nic.cz")
                    .exec(ctx);
            const unsigned long long future_begin_hid = ::LibFred::UpdateContactByHandle(tech_c1_handle, registrar.handle)
                .set_email("future.nsset1.tech.c.1@.nic.cz")
                .set_notifyemail("future.nsset1.tech.c.1.notify@.nic.cz")
                .exec(ctx);

            make_history_version_begin_older( ctx, crhid, history_starts_years_ago, true );
            make_history_version_end_older( ctx, crhid, notified_version_starts_years_ago);

            make_history_version_begin_older( ctx, to_be_notified_hid, notified_version_starts_years_ago, true );
            make_history_version_end_older( ctx, to_be_notified_hid, future_starts_years_ago);

            make_history_version_begin_older( ctx, future_begin_hid , future_starts_years_ago, true );

            nsset_tech_c1_to_be_notified = ::LibFred::InfoContactHistoryByHistoryid(to_be_notified_hid).exec(ctx).info_contact_data;
        }

        const std::string tech_c2_handle = "NSSET2_TECH_C2";
        {
            const unsigned long long crhid =
                Test::exec(
                    ::LibFred::CreateContact(tech_c2_handle, registrar.handle)
                        .set_email("history.nsset2.tech.c.2@.nic.cz")
                        .set_notifyemail("history.nsset2.tech.c.2.notify@.nic.cz"),
                    ctx
                ).historyid;
            const unsigned long long to_be_notified_hid =
                ::LibFred::UpdateContactByHandle(tech_c2_handle, registrar.handle)
                    .set_email("nsset2.tech.c.2@.nic.cz")
                    .set_notifyemail("nsset2.tech.c.2.notify@.nic.cz")
                    .exec(ctx);
            const unsigned long long future_begin_hid = ::LibFred::UpdateContactByHandle(tech_c2_handle, registrar.handle)
                .set_email("future.nsset2.tech.c.2@.nic.cz")
                .set_notifyemail("future.nsset2.tech.c.2.notify@.nic.cz")
                .exec(ctx);

            make_history_version_begin_older( ctx, crhid, history_starts_years_ago, true );
            make_history_version_end_older( ctx, crhid, notified_version_starts_years_ago);

            make_history_version_begin_older( ctx, to_be_notified_hid, notified_version_starts_years_ago, true );
            make_history_version_end_older( ctx, to_be_notified_hid, future_starts_years_ago);

            make_history_version_begin_older( ctx, future_begin_hid , future_starts_years_ago, true );

            nsset_tech_c2_to_be_notified = ::LibFred::InfoContactHistoryByHistoryid(to_be_notified_hid).exec(ctx).info_contact_data;
        }

        const std::string nsset_handle = "NSSET1";
        {
            const unsigned long long crhid =
                Test::exec(
                    ::LibFred::CreateNsset(nsset_handle, registrar.handle)
                        .set_tech_contacts(
                            boost::assign::list_of(tech_c1_handle)(tech_c2_handle)
                        ),
                    ctx
                ).crhistoryid;
            make_history_version_begin_older( ctx, crhid, notification_is_years_ago, true );
            nsset = ::LibFred::InfoNssetByHandle(nsset_handle).exec(ctx).info_nsset_data;
        }
    }
};

BOOST_FIXTURE_TEST_CASE(test_created_nsset, has_nsset)
{
    std::set<std::string> email_addresses;

    email_addresses.insert( nsset_tech_c1_to_be_notified.notifyemail.get_value() );
    email_addresses.insert( nsset_tech_c2_to_be_notified.notifyemail.get_value() );

    BOOST_CHECK(
        Notification::gather_email_addresses(
            ctx,
            Notification::EventOnObject(::LibFred::nsset, Notification::created),
            nsset.crhistoryid
        ) == email_addresses
    );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
