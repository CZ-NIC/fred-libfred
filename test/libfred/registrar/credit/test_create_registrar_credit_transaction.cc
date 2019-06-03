#include "libfred/registrar/credit/create_registrar_credit_transaction.hh"
#include "libfred/registrar/credit/exceptions.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/registrar/info_registrar_data.hh"
#include "libfred/registrar/zone_access/add_registrar_zone_access.hh"
#include "libfred/zone/create_zone.hh"

#include "libfred/db_settings.hh"
#include "libfred/opcontext.hh"
#include "util/random_data_generator.hh"
#include "test/libfred/util.hh"
#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>

void init_registrar_credit(::LibFred::OperationContext& _ctx,
        unsigned long long _registrar_id,
        unsigned long long _zone_id)
{
    _ctx.get_conn().exec_params(
            "INSERT INTO registrar_credit (registrar_id, zone_id, credit) "
            "VALUES ($1::bigint, $2::bigint, 0) "
            "ON CONFLICT DO NOTHING ",
            Database::query_param_list(_registrar_id)(_zone_id));
}

struct CreateRegistrarCreditTransactionFixture : virtual public Test::instantiate_db_template
{
    std::string zone_fqdn;
    Decimal change_credit;
    ::LibFred::InfoRegistrarData registrar;

    CreateRegistrarCreditTransactionFixture()
            : zone_fqdn(RandomDataGenerator().xstring(5)),
              change_credit(RandomDataGenerator().xnumstring(8))
    {
        ::LibFred::OperationContextCreator ctx;
        registrar = Test::registrar::make(ctx);
        ctx.commit_transaction();
    }
};

BOOST_FIXTURE_TEST_SUITE(TestCreateRegistrarCreditTransaction, CreateRegistrarCreditTransactionFixture)

BOOST_AUTO_TEST_CASE(registrar_credit_nonexistent_registrar)
{
    ::LibFred::OperationContextCreator ctx;
    const std::string nonexistent_registrar = RandomDataGenerator().xstring(15);
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
        ::LibFred::Registrar::Credit::NonexistentZone);
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

BOOST_AUTO_TEST_SUITE_END(); //TestCreateRegistrarCreditTransaction
