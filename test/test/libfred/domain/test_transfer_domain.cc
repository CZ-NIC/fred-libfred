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
#include <string>

#include "libfred/registrable_object/domain/transfer_domain.hh"
#include "libfred/object/transfer_object_exception.hh"
#include "libfred/exception.hh"

#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "test/libfred/util.hh"

BOOST_FIXTURE_TEST_SUITE(TestTransferDomain, Test::has_domain_and_a_different_registrar)

BOOST_AUTO_TEST_CASE(test_transfer_ok_data)
{
    const unsigned long long logd_request_id = 753159;

    const unsigned long long post_transfer_history_id = ::LibFred::TransferDomain(domain.id, the_different_registrar.handle, domain.authinfopw, logd_request_id).exec(ctx);

    const std::string timezone = "Europe/Prague";

    const ::LibFred::InfoDomainOutput post_transfer_domain_metadata = ::LibFred::InfoDomainById(domain.id).exec(ctx);
    const ::LibFred::InfoDomainData& post_transfer_domain_data = post_transfer_domain_metadata.info_domain_data;

    BOOST_CHECK_EQUAL(
        ::LibFred::InfoRegistrarByHandle(post_transfer_domain_data.sponsoring_registrar_handle).exec(ctx).info_registrar_data.id,
        the_different_registrar.id
    );

    BOOST_CHECK_EQUAL(
        post_transfer_domain_data.transfer_time,
        boost::posix_time::time_from_string(
            static_cast<std::string>(
                ctx.get_conn().exec_params(
                    "SELECT now()::TIMESTAMP AT TIME ZONE 'UTC' AT TIME ZONE $1::text",
                    Database::query_param_list(timezone)
                )[0][0]
            )
        )
    );

    BOOST_CHECK_EQUAL(
        post_transfer_history_id,
        post_transfer_domain_data.historyid
    );

    BOOST_CHECK_EQUAL(
        logd_request_id,
        post_transfer_domain_metadata.logd_request_id
    );

    BOOST_CHECK_EQUAL(
        static_cast<unsigned long long>(
            ctx.get_conn().exec_params(
                "SELECT MAX(historyid) AS current_hid_ FROM domain_history WHERE id = $1::integer",
                Database::query_param_list(domain.id)
            )[0]["current_hid_"]
        ),
        post_transfer_history_id
    );

    BOOST_CHECK_EQUAL(
        post_transfer_domain_data,
        ::LibFred::InfoDomainHistoryById(domain.id).exec(ctx).at(0).info_domain_data
    );
}

BOOST_FIXTURE_TEST_CASE(test_transfer_ok_data_enum, Test::has_enum_domain_and_a_different_registrar)
{
    const unsigned long long logd_request_id = 753159;

    const unsigned long long post_transfer_history_id = ::LibFred::TransferDomain(domain.id, the_different_registrar.handle, domain.authinfopw, logd_request_id).exec(ctx);

    const std::string timezone = "Europe/Prague";

    const ::LibFred::InfoDomainOutput post_transfer_domain_metadata = ::LibFred::InfoDomainById(domain.id).exec(ctx);
    const ::LibFred::InfoDomainData& post_transfer_domain_data = post_transfer_domain_metadata.info_domain_data;

    BOOST_CHECK_EQUAL(
        ::LibFred::InfoRegistrarByHandle(post_transfer_domain_data.sponsoring_registrar_handle).exec(ctx).info_registrar_data.id,
        the_different_registrar.id
    );

    BOOST_CHECK_EQUAL(
        post_transfer_domain_data.transfer_time,
        boost::posix_time::time_from_string(
            static_cast<std::string>(
                ctx.get_conn().exec_params(
                    "SELECT now()::TIMESTAMP AT TIME ZONE 'UTC' AT TIME ZONE $1::text",
                    Database::query_param_list(timezone)
                )[0][0]
            )
        )
    );

    BOOST_CHECK_EQUAL(
        post_transfer_history_id,
        post_transfer_domain_data.historyid
    );

    BOOST_CHECK_EQUAL(
        logd_request_id,
        post_transfer_domain_metadata.logd_request_id
    );

    BOOST_CHECK_EQUAL(
        static_cast<unsigned long long>(
            ctx.get_conn().exec_params(
                "SELECT MAX(historyid) AS current_hid_ FROM domain_history WHERE id = $1::integer",
                Database::query_param_list(domain.id)
            )[0]["current_hid_"]
        ),
        post_transfer_history_id
    );

    BOOST_CHECK_EQUAL(
        post_transfer_domain_data,
        ::LibFred::InfoDomainHistoryById(domain.id).exec(ctx).at(0).info_domain_data
    );
}

BOOST_AUTO_TEST_CASE(test_transfer_ok_by_domain_authinfo)
{
    ::LibFred::TransferDomain transfer(
        domain.id, the_different_registrar.handle, domain.authinfopw, Nullable<unsigned long long>()
    );

    BOOST_CHECK_NO_THROW( transfer.exec(ctx) );
}

BOOST_AUTO_TEST_CASE(test_transfer_ok_by_registrant_authinfo)
{
    ::LibFred::TransferDomain transfer(
        domain.id, the_different_registrar.handle, contact.authinfopw, Nullable<unsigned long long>()
    );

    BOOST_CHECK_NO_THROW( transfer.exec(ctx) );
}

BOOST_AUTO_TEST_CASE(test_transfer_ok_by_admin_contact)
{
    ::LibFred::TransferDomain transfer(
        domain.id, the_different_registrar.handle, admin_contact1.authinfopw, Nullable<unsigned long long>()
    );

    BOOST_CHECK_NO_THROW( transfer.exec(ctx) );
}

BOOST_AUTO_TEST_CASE(test_unknown_registrar)
{
    ::LibFred::TransferDomain transfer(
        domain.id,
        "nonexistentregistrar", /* <= !!! */
        domain.authinfopw,
        Nullable<unsigned long long>()
    );

    BOOST_CHECK_THROW(
        transfer.exec(ctx),
        ::LibFred::UnknownRegistrar
    );
}

BOOST_AUTO_TEST_CASE(test_unknown_object)
{
    ::LibFred::TransferDomain transfer(
        Test::get_nonexistent_object_id(ctx), /* <= !!! */
        the_different_registrar.handle,
        domain.authinfopw,
        Nullable<unsigned long long>()
    );

    BOOST_CHECK_THROW(
        transfer.exec(ctx),
        ::LibFred::UnknownDomainId
    );
}

BOOST_AUTO_TEST_CASE(test_incorrect_authinfopw)
{
    ::LibFred::TransferDomain transfer(
        domain.id,
        the_different_registrar.handle,
        "IwouldBEverySURPRISEDifANYONEhasSUCHauthinfopw486413514543154144178743404566387036415415051", /* <= !!! */
        Nullable<unsigned long long>()
    );
    BOOST_CHECK_THROW(
        transfer.exec(ctx),
        ::LibFred::IncorrectAuthInfoPw
    );
}

struct has_domain_and_a_different_registrar_and_a_different_contact : Test::has_domain_and_a_different_registrar {

    ::LibFred::InfoContactData different_contact;

    has_domain_and_a_different_registrar_and_a_different_contact() {
        const std::string different_contact_handle = "THE-DIFFERENT-ONE";
        ::LibFred::CreateContact(different_contact_handle, registrar.handle).exec(ctx);
        different_contact = ::LibFred::InfoContactByHandle(different_contact_handle).exec(ctx).info_contact_data;
    }
};

BOOST_FIXTURE_TEST_CASE(test_incorrect_authinfopw_other_contact, has_domain_and_a_different_registrar_and_a_different_contact)
{
    ::LibFred::TransferDomain transfer(
        domain.id,
        the_different_registrar.handle,
        different_contact.handle, /* <= !!! */
        Nullable<unsigned long long>()
    );
    BOOST_CHECK_THROW(
        transfer.exec(ctx),
        ::LibFred::IncorrectAuthInfoPw
    );
}

BOOST_AUTO_TEST_CASE(test_registrar_is_already_sponsoring)
{
    ::LibFred::TransferDomain transfer(
        domain.id,
        registrar.handle, /* <= !!! */
        domain.authinfopw,
        Nullable<unsigned long long>()
    );
    BOOST_CHECK_THROW(
        transfer.exec(ctx),
        ::LibFred::NewRegistrarIsAlreadySponsoring
    );
}

BOOST_AUTO_TEST_SUITE_END()