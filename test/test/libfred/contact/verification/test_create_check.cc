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
 *  integration tests for CreateContactCheck operation
 */

#include "libfred/registrable_object/contact/verification/create_check.hh"
#include "libfred/registrable_object/contact/verification/info_check.hh"
#include "libfred/registrable_object/contact/verification/enum_check_status.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/db_settings.hh"
#include "util/db/nullable.hh"

#include "test/libfred/contact/verification/setup_utils.hh"
#include "test/setup/fixtures.hh"

#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>

#include <vector>
#include <utility>
#include <string>

BOOST_AUTO_TEST_SUITE(TestContactVerification)
BOOST_FIXTURE_TEST_SUITE(TestCreateContactCheck_integ, Test::instantiate_db_template)

const std::string server_name = "test-contact_verification-create_check_integ";

/**
 executing CreateContactCheck with only mandatory setup
 @pre valid contact handle
 @pre valid testsuite handle including one existing test
 @post correct values present in InfoContactCheckOutput::to_string()
 */
BOOST_AUTO_TEST_CASE(test_Exec_mandatory_setup)
{
    Test::contact contact;
    setup_testsuite testsuite;

    ::LibFred::CreateContactCheck create_check(contact.info_data.id, testsuite.testsuite_handle);
    std::string handle;
    const std::string timezone = "UTC";

    try
    {
        ::LibFred::OperationContextCreator ctx1;
        handle = create_check.exec(ctx1);
        ctx1.commit_transaction();
    }
    catch (const ::LibFred::InternalError& exp)
    {
        BOOST_FAIL("exception (1):" + boost::diagnostic_information(exp) + exp.what());
    }
    catch (const boost::exception& exp)
    {
        BOOST_FAIL("exception (2):" + boost::diagnostic_information(exp));
    } catch(const std::exception& exp)
    {
        BOOST_FAIL(std::string("exception (3):") + exp.what());
    }

    ::LibFred::InfoContactCheck info_check( uuid::from_string(handle) );
    ::LibFred::InfoContactCheckOutput result_data;

    try
    {
        ::LibFred::OperationContextCreator ctx2;
        result_data = info_check.exec(ctx2, timezone);
    }
    catch (const ::LibFred::InternalError& exp)
    {
        BOOST_FAIL("exception (1):" + boost::diagnostic_information(exp) + exp.what() );
    }
    catch (const boost::exception& exp)
    {
        BOOST_FAIL("exception (2):" + boost::diagnostic_information(exp));
    }
    catch (const std::exception& exp)
    {
        BOOST_FAIL(std::string("exception (3):") + exp.what());
    }

    // create_time is reasonable

    ptime now = second_clock::universal_time();
    ptime create_time_min = now - minutes(1);
    ptime create_time_max = now + minutes(1);

    BOOST_CHECK_MESSAGE(
            create_time_min < result_data.local_create_time,
            "invalid contact_check.create_time: " + boost::posix_time::to_simple_string(result_data.local_create_time) +
            " 'now' is:" + boost::posix_time::to_simple_string(now));

    BOOST_CHECK_MESSAGE(
            result_data.local_create_time < create_time_max,
            "invalid contact_check.create_time: " + boost::posix_time::to_simple_string(result_data.local_create_time) +
            " 'now' is:" + boost::posix_time::to_simple_string(now) );

    // contact_history_id is correct regarding the create_time
    ::LibFred::OperationContextCreator ctx3;
    const Database::Result contact_history_validity_interval = ctx3.get_conn().exec_params(
        "SELECT valid_from AT TIME ZONE 'utc' AT TIME ZONE $1::text,"
               "valid_to AT TIME ZONE 'utc' AT TIME ZONE $1::text "
        "FROM history "
        "WHERE id=$2::int",
        Database::query_param_list
            (timezone)
            (result_data.contact_history_id));

    BOOST_REQUIRE_MESSAGE(
        contact_history_validity_interval.size() == 1,
        "failed to get appropriate history id \n" + result_data.to_string());

    const boost::posix_time::ptime from_time = boost::posix_time::time_from_string(
            static_cast<std::string>(contact_history_validity_interval[0][0]));

    boost::posix_time::ptime to_time;
    const bool to_is_null = contact_history_validity_interval[0][1].isnull();
    if (!to_is_null)
    {
        to_time = boost::posix_time::time_from_string(
                static_cast<std::string>(contact_history_validity_interval[0][1]));
    }

    /* WARNING - "...OR EQUAL"
     * part of comparison is present because the contact is created in the same transaction
     * and therefore has the same value of NOW() time function in postgres
     */
    BOOST_CHECK_MESSAGE(
        (from_time <= result_data.local_create_time) &&
        ((result_data.local_create_time <= to_time) || to_is_null),
        "invalid history_id - not valid at check create_time." +
        result_data.to_string() +
        "expected interval is: " + boost::posix_time::to_simple_string(from_time) +
        " - " + boost::posix_time::to_simple_string(to_time) + "\n");

    // logd_request_id

    BOOST_CHECK_MESSAGE(
        result_data.check_state_history.begin()->logd_request_id.isnull(),
        "logd_request_id should be NULL");

    // testsuite_id
    BOOST_CHECK_MESSAGE(
        result_data.testsuite_handle == testsuite.testsuite_handle,
        "testsuite handle differs in CreateContactCheck (" + testsuite.testsuite_handle + ") & InfoContactCheck (" +
        testsuite.testsuite_handle + ")");

    // create_time
    BOOST_CHECK_MESSAGE(
         result_data.local_create_time == result_data.check_state_history.begin()->local_update_time,
         "create_time (" + boost::posix_time::to_simple_string(result_data.local_create_time) + ") "
         "differs from first update_time (" +
         boost::posix_time::to_simple_string(result_data.check_state_history.begin()->local_update_time) + ")");

    // status handle is 'ENQUEUE_REQ'
    BOOST_CHECK_MESSAGE(
         result_data.check_state_history.begin()->status_handle == ::LibFred::ContactCheckStatus::ENQUEUE_REQ,
         "status of create check (" + result_data.check_state_history.begin()->status_handle + ") should be " +
         ::LibFred::ContactCheckStatus::ENQUEUE_REQ);
}

/**
 executing CreateContactCheck with full mandatory + optional setup
 @pre valid contact handle
 @pre valid testsuite handle including one existing test
 @post correct values present in InfoContactCheckOutput::to_string()
 */
BOOST_AUTO_TEST_CASE(test_Exec_optional_setup)
{
    Test::contact contact;
    setup_testsuite testsuite;
    setup_logd_request_id logd_request;

    ::LibFred::CreateContactCheck create_check(contact.info_data.id, testsuite.testsuite_handle, logd_request.logd_request_id);
    std::string handle;
    const std::string timezone = "UTC";

    try
    {
        ::LibFred::OperationContextCreator ctx1;
        handle = create_check.exec(ctx1);
        ctx1.commit_transaction();
    }
    catch (const ::LibFred::InternalError& exp)
    {
        BOOST_FAIL("exception (1):" + boost::diagnostic_information(exp) + exp.what() );
    }
    catch (const boost::exception& exp)
    {
        BOOST_FAIL("exception (2):" + boost::diagnostic_information(exp));
    }
    catch (const std::exception& exp)
    {
        BOOST_FAIL(std::string("exception (3):") + exp.what());
    }

    ::LibFred::InfoContactCheck info_check(uuid::from_string(handle));
    ::LibFred::InfoContactCheckOutput result_data;

    try
    {
        ::LibFred::OperationContextCreator ctx2;
        result_data = info_check.exec(ctx2, timezone);
    }
    catch (const ::LibFred::InternalError& exp)
    {
        BOOST_FAIL("exception (1):" + boost::diagnostic_information(exp) + exp.what() );
    }
    catch (const boost::exception& exp)
    {
        BOOST_FAIL("exception (2):" + boost::diagnostic_information(exp));
    }
    catch (const std::exception& exp)
    {
        BOOST_FAIL(std::string("exception (3):") + exp.what());
    }

    // create_time is reasonable

    const ptime now = second_clock::universal_time();
    const ptime create_time_min = now - minutes(1);
    const ptime create_time_max = now + minutes(1);

    BOOST_CHECK_MESSAGE(
        result_data.local_create_time > create_time_min,
        "invalid contact_check.create_time: " + boost::posix_time::to_simple_string(result_data.local_create_time) +
        " 'now' is:" + boost::posix_time::to_simple_string(now));

    BOOST_CHECK_MESSAGE(
        result_data.local_create_time < create_time_max,
        "invalid contact_check.create_time: " + boost::posix_time::to_simple_string(result_data.local_create_time) +
        " 'now' is:" + boost::posix_time::to_simple_string(now));

    // contact_history_id is correct regarding the create_time

    ::LibFred::OperationContextCreator ctx3;
    const Database::Result contact_history_validity_interval = ctx3.get_conn().exec_params(
        "SELECT valid_from AT TIME ZONE 'utc' AT TIME ZONE $1::varchar,"
               "valid_to  AT TIME ZONE 'utc' AT TIME ZONE $1::varchar "
        "FROM history "
        "WHERE id=$2::int; ",
        Database::query_param_list
            (timezone)
            (result_data.contact_history_id));

    BOOST_REQUIRE_MESSAGE(
        contact_history_validity_interval.size() == 1,
        "failed to get appropriate history id \n" + result_data.to_string());

    const boost::posix_time::ptime from_time = boost::posix_time::time_from_string(
            static_cast<std::string> (contact_history_validity_interval[0][0]));

    boost::posix_time::ptime to_time;
    const bool to_is_null = contact_history_validity_interval[0][1].isnull();
    if (!to_is_null)
    {
        to_time = boost::posix_time::time_from_string(
                static_cast<std::string>(contact_history_validity_interval[0][1]));
    }
    /* WARNING - "...OR EQUAL"
         * part of comparison is present because the contact is created in the same transaction
         * and therefore has the same value of NOW() time function in postgres
         */
    BOOST_CHECK_MESSAGE(
        (from_time <= result_data.local_create_time) &&
        ((result_data.local_create_time <= to_time) || to_is_null),
        "invalid history_id - not valid at check create_time." + result_data.to_string());

    // logd_request_id

    BOOST_CHECK_MESSAGE(
        result_data.check_state_history.begin()->logd_request_id.get_value_or_default() == logd_request.logd_request_id,
        "logd_request_id differs in CreateContactCheck (" + boost::lexical_cast<std::string>(logd_request.logd_request_id) +
        ") & InfoContactCheck (" + result_data.check_state_history.begin()->logd_request_id.print_quoted() + ")");

    // testsuite_id
    BOOST_CHECK_MESSAGE(
        result_data.testsuite_handle == testsuite.testsuite_handle,
        "testsuite handle differs in CreateContactCheck (" + testsuite.testsuite_handle + ") & InfoContactCheck (" +
        testsuite.testsuite_handle +")");

    // create_time
    BOOST_CHECK_MESSAGE(
         result_data.local_create_time == result_data.check_state_history.begin()->local_update_time,
         "create_time (" + boost::posix_time::to_simple_string(result_data.local_create_time) + ") differs from first "
         "update_time (" + boost::posix_time::to_simple_string(result_data.check_state_history.begin()->local_update_time) + ")");

    // status handle is 'ENQUEUE_REQ'
    BOOST_CHECK_MESSAGE(
         result_data.check_state_history.begin()->status_handle == ::LibFred::ContactCheckStatus::ENQUEUE_REQ,
         "status of create check (" + result_data.check_state_history.begin()->status_handle + ") should be " +
         ::LibFred::ContactCheckStatus::ENQUEUE_REQ);
}

/**
 setting nonexistent contact handle value and executing operation
 @pre nonexistent contact handle
 @pre valid testsuite handle
 @post ::LibFred::CreateContactCheck::ExceptionUnknownContactHandle
*/
BOOST_AUTO_TEST_CASE(test_Exec_nonexistent_contact_id)
{
    setup_testsuite testsuite;

    try
    {
        ::LibFred::OperationContextCreator ctx;
        ::LibFred::CreateContactCheck create_check(Test::get_nonexistent_object_id(ctx), testsuite.testsuite_handle);
        create_check.exec(ctx);
        ctx.commit_transaction();
        BOOST_FAIL("should have caught the exception");
    }
    catch (const ::LibFred::ExceptionUnknownContactId&)
    {
        return;
    }
    catch (const Database::ResultFailed& e)
    {
        BOOST_FAIL(std::string("incorrect exception caught \n") + boost::diagnostic_information(e));
    }
    catch (const std::exception& e)
    {
        BOOST_FAIL(std::string("incorrect exception caught \n") + boost::diagnostic_information(e));
    }
    catch (...)
    {
        BOOST_FAIL("incorrect exception caught \n");
    }
}

/**
 setting nonexistent testsuite handle value and executing operation
 @pre valid contact handle
 @pre nonexistent testsuite handle
 @post ::LibFred::CreateContactCheck::ExceptionUnknownTestsuiteHandle
 */
BOOST_AUTO_TEST_CASE(test_Exec_nonexistent_testsuite_handle)
{
    Test::contact contact;
    setup_nonexistent_testsuite_handle testsuite;

    ::LibFred::CreateContactCheck create_check(contact.info_data.id, testsuite.testsuite_handle);

    try
    {
        ::LibFred::OperationContextCreator ctx;
        create_check.exec(ctx);
        ctx.commit_transaction();
        BOOST_FAIL("should have caught the exception");
    }
    catch (const ::LibFred::ExceptionUnknownTestsuiteHandle&)
    {
        return;
    }
    catch (...)
    {
        BOOST_FAIL("incorrect exception caught");
    }
}

BOOST_AUTO_TEST_SUITE_END()//TestContactVerification/TestCreateContactCheck_integ
BOOST_AUTO_TEST_SUITE_END()//TestContactVerification
