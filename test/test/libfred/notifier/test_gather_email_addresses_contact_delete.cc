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
BOOST_AUTO_TEST_SUITE(Contact)
BOOST_AUTO_TEST_SUITE(Delete)

/* time: -inf ---> history ---> notified_version ---> +inf */
const unsigned history_starts_years_ago = 4;
const unsigned notification_is_years_ago = 2;

struct has_contact : has_autocomitting_ctx {
    ::LibFred::InfoRegistrarData registrar;
    ::LibFred::InfoContactData cont;

    has_contact() {
        const std::string cont_handle = "CONTACT1";
        registrar = Test::registrar(ctx).info_data;

        const unsigned long long crhid =
            Test::exec(
                ::LibFred::CreateContact(cont_handle, registrar.handle)
                    .set_email("history.contact.1@nic.cz")
                    .set_notifyemail("history.contact.1.notify@.nic.cz"),
                ctx
            ).crhistoryid;

        const unsigned long long hid_to_be_notified =
            ::LibFred::UpdateContactByHandle(cont_handle, registrar.handle)
                .set_email("contact.1%nic.cz")
                .set_notifyemail("contact.1.notify#nic.cz")
                .exec(ctx);

        make_history_version_begin_older( ctx, crhid, history_starts_years_ago, true );
        make_history_version_end_older( ctx, crhid, notification_is_years_ago);
        make_history_version_begin_older( ctx, hid_to_be_notified, notification_is_years_ago, false );

        cont = ::LibFred::InfoContactHistoryByHistoryid(hid_to_be_notified).exec(ctx).info_contact_data;
    }
};

struct has_deleted_contact : public has_contact {
    has_deleted_contact() {
        ::LibFred::DeleteContactByHandle(cont.handle).exec(ctx);
    }
};

BOOST_FIXTURE_TEST_CASE(test_deleted_contact, has_deleted_contact)
{
    std::set<std::string> email_addresses;

    email_addresses.insert( cont.notifyemail.get_value() );

    BOOST_CHECK(
        Notification::gather_email_addresses(
            ctx,
            Notification::EventOnObject(::LibFred::contact, Notification::deleted),
            cont.historyid
        ) == email_addresses
    );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
