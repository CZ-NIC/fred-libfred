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
#include "test/libfred/notifier/fixture_data.hh"

#include "libfred/notifier/gather_email_data/gather_email_content.hh"

BOOST_AUTO_TEST_SUITE(TestNotifier)
BOOST_AUTO_TEST_SUITE(GatherEmailContent)
BOOST_AUTO_TEST_SUITE(Keyset)
BOOST_AUTO_TEST_SUITE(Create)

BOOST_FIXTURE_TEST_CASE(test_empty_create, has_empty_keyset)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "4";
    etalon["handle"] = keyset.handle;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::keyset, Notification::created),
                registrar.id,
                keyset.crhistoryid,
                input_svtrid
            )
        )
    );
}

BOOST_FIXTURE_TEST_CASE(test_full_create, has_full_keyset)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "4";
    etalon["handle"] = keyset.handle;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::keyset, Notification::created),
                registrar.id,
                keyset.crhistoryid,
                input_svtrid
            )
        )
    );
}

BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();
