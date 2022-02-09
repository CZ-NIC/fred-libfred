/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/credit/create_registrar_credit_transaction.hh"
#include "libfred/registrar/credit/exceptions.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/registrar/info_registrar_data.hh"
#include "libfred/registrar/zone_access/add_registrar_zone_access.hh"
#include "libfred/registrar/zone_access/registrar_zone_access_history.hh"
#include "libfred/zone/create_zone.hh"
#include "libfred/zone/exceptions.hh"

#include "libfred/db_settings.hh"
#include "libfred/opcontext.hh"
#include "test/libfred/util.hh"
#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>

namespace {

void init_registrar_credit(::LibFred::OperationContext& _ctx,
        unsigned long long _registrar_id,
        unsigned long long _zone_id)
{
    _ctx.get_conn().exec_params(
            "INSERT INTO registrar_credit (registrar_id, zone_id, credit) "
            "VALUES ($1::bigint, $2::bigint, 0) "
            "ON CONFLICT DO NOTHING",
            Database::query_param_list(_registrar_id)(_zone_id));
}

struct CreateRegistrarCreditTransactionFixture : virtual public Test::instantiate_db_template
{
    std::string zone_fqdn;
    Decimal change_credit;
    ::LibFred::InfoRegistrarData registrar;

    CreateRegistrarCreditTransactionFixture()
        : zone_fqdn(Random::Generator().get_seq(Random::CharSet::letters(), 5)),
          change_credit(Random::Generator().get_seq(Random::CharSet::digits(), 8))
    {
        ::LibFred::OperationContextCreator ctx;
        registrar = Test::registrar::make(ctx);
        ctx.commit_transaction();
    }
};

struct EmptyFixture { };

}//namespace {anonymous}

BOOST_FIXTURE_TEST_SUITE(TestCreateRegistrarCreditTransaction, CreateRegistrarCreditTransactionFixture)

BOOST_FIXTURE_TEST_CASE(registrar_zone_access_history_test, EmptyFixture)
{
    ::LibFred::Registrar::ZoneAccess::RegistrarZoneAccessHistory registrar_zone_access_history;
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    ::LibFred::Registrar::ZoneAccess::TimeInterval time_interval;
    time_interval.from_date = boost::gregorian::from_string("2019-05-13");
    time_interval.to_date = boost::none;
    auto invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(1ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["aBc"].size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    // <2019-05-13, oo)
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));

    registrar_zone_access_history.invoices_by_zone.clear();
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 0);
    time_interval.from_date = boost::gregorian::from_string("2019-05-13");
    time_interval.to_date = boost::gregorian::from_string("2019-06-13");
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 1);
    // <2019-05-13, 2019-06-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));

    time_interval.from_date = boost::gregorian::from_string("2019-07-13");
    time_interval.to_date = boost::none;
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(2ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 2);
    // <2019-05-13, 2019-06-13>, <2019-07-13, oo)
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-14")));

    registrar_zone_access_history.invoices_by_zone.clear();
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 0);
    time_interval.from_date = boost::gregorian::from_string("2019-05-13");
    time_interval.to_date = boost::gregorian::from_string("2019-06-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(1ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 1);
    time_interval.from_date = boost::gregorian::from_string("2019-07-13");
    time_interval.to_date = boost::gregorian::from_string("2019-08-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(2ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 2);
    // <2019-05-13, 2019-06-13>, <2019-07-13, 2019-08-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-09-14")));

    registrar_zone_access_history.invoices_by_zone.clear();
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 0);
    time_interval.from_date = boost::gregorian::from_string("2019-07-13");
    time_interval.to_date = boost::gregorian::from_string("2019-08-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(2ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 1);
    time_interval.from_date = boost::gregorian::from_string("2019-05-13");
    time_interval.to_date = boost::gregorian::from_string("2019-06-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(1ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 2);
    // <2019-05-13, 2019-06-13>, <2019-07-13, 2019-08-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-07-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-08-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-09-14")));

    registrar_zone_access_history.invoices_by_zone.clear();
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 0);
    time_interval.from_date = boost::gregorian::from_string("2019-05-13");
    time_interval.to_date = boost::gregorian::from_string("2019-05-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(1ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 1);
    // <2019-05-13, 2019-05-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));

    time_interval.from_date = boost::gregorian::from_string("2019-06-13");
    time_interval.to_date = boost::gregorian::from_string("2019-06-13");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(3ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 2);
    // <2019-05-13, 2019-05-13>, <2019-06-13, 2019-06-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-12")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-04-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));

    time_interval.from_date = boost::gregorian::from_string("2019-05-14");
    time_interval.to_date = boost::gregorian::from_string("2019-06-12");
    invoice_id = Util::make_strong<::LibFred::Registrar::ZoneAccess::RegistrarInvoiceId>(2ull);
    registrar_zone_access_history.invoices_by_zone["abc"].insert({time_interval, invoice_id});
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone.size(), 1);
    BOOST_CHECK_EQUAL(registrar_zone_access_history.invoices_by_zone["abc"].size(), 3);
    // <2019-05-13, 2019-05-13>, <2019-05-14, 2019-06-12>, <2019-06-13, 2019-06-13>
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-13")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-14")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-05-15")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-11")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-12")));
    BOOST_CHECK(has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-13")));
    BOOST_CHECK(!has_access(registrar_zone_access_history, "abc", boost::gregorian::from_string("2019-06-14")));
}

BOOST_AUTO_TEST_CASE(registrar_credit_nonexistent_registrar)
{
    ::LibFred::OperationContextCreator ctx;
    const std::string nonexistent_registrar = Random::Generator().get_seq(Random::CharSet::letters(), 15);
    BOOST_CHECK_THROW(
        ::LibFred::Registrar::Credit::CreateRegistrarCreditTransaction(nonexistent_registrar, zone_fqdn, change_credit)
            .exec(ctx),
        ::LibFred::Registrar::Credit::NonexistentRegistrar);
}

BOOST_AUTO_TEST_CASE(registrar_credit_nonexistent_zone)
{
    ::LibFred::OperationContextCreator ctx;
    BOOST_CHECK_THROW(
        ::LibFred::Registrar::Credit::CreateRegistrarCreditTransaction(registrar.handle, zone_fqdn, change_credit)
            .exec(ctx),
        ::LibFred::Zone::NonExistentZone);
}

BOOST_AUTO_TEST_CASE(registrar_credit_nonexistent_zone_access)
{
    ::LibFred::OperationContextCreator ctx;
    ::LibFred::Zone::CreateZone(zone_fqdn, 6, 12).exec(ctx);
    BOOST_CHECK_THROW(
        ::LibFred::Registrar::Credit::CreateRegistrarCreditTransaction(registrar.handle, zone_fqdn, change_credit)
            .exec(ctx),
        ::LibFred::Registrar::Credit::NonexistentZoneAccess);
}

BOOST_AUTO_TEST_CASE(create_registrar_first_credit_transaction)
{
    ::LibFred::OperationContextCreator ctx;
    ::LibFred::Zone::CreateZone(zone_fqdn, 6, 12).exec(ctx);
    boost::gregorian::date date_from(boost::gregorian::day_clock::local_day());
    ::LibFred::Registrar::ZoneAccess::AddRegistrarZoneAccess(registrar.handle, zone_fqdn, date_from).exec(ctx);
    ::LibFred::Registrar::Credit::CreateRegistrarCreditTransaction(registrar.handle, zone_fqdn, change_credit)
            .exec(ctx);
}

BOOST_AUTO_TEST_CASE(create_registrar_credit_transaction)
{
    ::LibFred::OperationContextCreator ctx;
    const unsigned long long zone_id = ::LibFred::Zone::CreateZone(zone_fqdn, 6, 12).exec(ctx);
    boost::gregorian::date date_from(boost::gregorian::day_clock::local_day());
    ::LibFred::Registrar::ZoneAccess::AddRegistrarZoneAccess(registrar.handle, zone_fqdn, date_from).exec(ctx);
    init_registrar_credit(ctx, registrar.id, zone_id);
    ::LibFred::Registrar::Credit::CreateRegistrarCreditTransaction(registrar.handle, zone_fqdn, change_credit)
            .exec(ctx);
}

BOOST_AUTO_TEST_SUITE_END()//TestCreateRegistrarCreditTransaction
