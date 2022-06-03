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

#include <boost/test/unit_test.hpp>

#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "test/libfred/notifier/util.hh"
#include "test/libfred/notifier/fixture_data.hh"

#include "libfred/notifier/gather_email_data/gather_email_content.hh"
#include "libfred/registrable_object/contact/transfer_contact.hh"

BOOST_AUTO_TEST_SUITE(TestNotifier)
BOOST_AUTO_TEST_SUITE(GatherEmailContent)
BOOST_AUTO_TEST_SUITE(Contact)
BOOST_AUTO_TEST_SUITE(Transfer)

template <typename HasContact>
struct has_contact_transferred : HasContact
{
    has_contact_transferred()
        : logd_request_id{12345},
          new_registrar{Test::registrar{HasContact::ctx}.info_data},
          new_historyid{
                ::LibFred::TransferContact{HasContact::contact.id, new_registrar.handle, HasContact::authinfopw}
                        .exec(HasContact::ctx)}
    { }
    unsigned long long logd_request_id;
    ::LibFred::InfoRegistrarData new_registrar;
    unsigned long long new_historyid;
};

BOOST_FIXTURE_TEST_CASE(test_transfer_empty, has_contact_transferred<HasEmptyContactWithAuthinfo>)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "1";
    etalon["handle"] = contact.handle;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::contact, Notification::transferred),
                registrar.id,
                new_historyid,
                input_svtrid
            )
        )
    );
}

BOOST_FIXTURE_TEST_CASE(test_transfer_full, has_contact_transferred<HasFullContactWithAuthinfo>)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "1";
    etalon["handle"] = contact.handle;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::contact, Notification::transferred),
                registrar.id,
                new_historyid,
                input_svtrid
            )
        )
    );
}

BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent/Contact/Transfer
BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent/Contact
BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent
BOOST_AUTO_TEST_SUITE_END()//TestNotifier
