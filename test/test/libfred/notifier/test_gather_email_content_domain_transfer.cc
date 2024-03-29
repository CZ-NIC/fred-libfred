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
#include "libfred/registrable_object/domain/transfer_domain.hh"

BOOST_AUTO_TEST_SUITE(TestNotifier)
BOOST_AUTO_TEST_SUITE(GatherEmailContent)
BOOST_AUTO_TEST_SUITE(Domain)
BOOST_AUTO_TEST_SUITE(Transfer)

template <typename HasDomain>
struct has_domain_transferred : HasDomain
{
    has_domain_transferred()
        : logd_request_id{12345},
          new_registrar{Test::registrar{HasDomain::ctx}.info_data},
          new_historyid{
                ::LibFred::TransferDomain{
                        HasDomain::dom.id,
                        new_registrar.handle,
                        HasDomain::authinfopw,
                        logd_request_id}
                .exec(HasDomain::ctx)}
    { }
    unsigned long long logd_request_id;
    ::LibFred::InfoRegistrarData new_registrar;
    unsigned long long new_historyid;
};

BOOST_FIXTURE_TEST_CASE(test_transfer_empty, has_domain_transferred<HasDomainWithAuthinfo>)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "3";
    etalon["handle"] = dom.fqdn;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::domain, Notification::transferred),
                registrar.id,
                new_historyid,
                input_svtrid
            )
        )
    );
}

BOOST_FIXTURE_TEST_CASE(test_transfer_enum, has_domain_transferred<HasEnumDomainWithAuthinfo>)
{
    const std::string input_svtrid = "abc-123";

    std::map<std::string, std::string> etalon;
    etalon["type"] = "3";
    etalon["handle"] = dom.fqdn;
    etalon["ticket"] = input_svtrid;
    etalon["registrar"] = registrar.name.get_value() + " (" + registrar.url.get_value() + ")";

    check_maps_are_equal(
        etalon,
        Notification::gather_email_content(
            ctx,
            Notification::notification_request(
                Notification::EventOnObject(::LibFred::domain, Notification::transferred),
                registrar.id,
                new_historyid,
                input_svtrid
            )
        )
    );
}

BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent/Domain/Transfer
BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent/Domain
BOOST_AUTO_TEST_SUITE_END()//TestNotifier/GatherEmailContent
BOOST_AUTO_TEST_SUITE_END()//TestNotifier
