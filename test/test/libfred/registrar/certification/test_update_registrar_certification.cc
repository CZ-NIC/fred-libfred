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

#include "libfred/registrar/certification/create_registrar_certification.hh"
#include "libfred/registrar/certification/update_registrar_certification.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "libfred/opcontext.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/test/unit_test.hpp>

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

struct test_update_certification_fixture : virtual public Test::instantiate_db_template
{
    test_update_certification_fixture()
    {
        LibFred::OperationContextCreator ctx;
        test_registrar = Test::registrar::make(ctx);
        valid_from = boost::gregorian::day_clock::local_day() + boost::gregorian::date_duration(1);
        valid_until = valid_from + boost::gregorian::date_duration(1);
        const auto dbres = ctx.get_conn().exec(
                "INSERT INTO files "
                       "(name, "
                        "path, "
                        "filesize, "
                        "filetype) "
                "VALUES ('update_file', "
                        "CONCAT(TO_CHAR(current_timestamp, 'YYYY/fmMM/fmD'), "
                               "'/', "
                               "CURRVAL('files_id_seq'::regclass))::TEXT, "
                        "0, "
                        "6) "
             "RETURNING id, uuid");
        file_id = static_cast<unsigned long long>(dbres[0][0]);
        file_uuid = {to_uuid(dbres[0][1])};
        certification_id = LibFred::Registrar::CreateRegistrarCertification(test_registrar.id,
                valid_from, score, file_uuid)
            .exec(ctx);
        ctx.commit_transaction();
    }
    LibFred::InfoRegistrarData test_registrar;
    unsigned long long certification_id;
    boost::gregorian::date valid_from;
    boost::gregorian::date valid_until;
    static const int score;
    LibFred::Registrar::FileUuid file_uuid;
    unsigned long long file_id;
};

const int test_update_certification_fixture::score = 1;

}//namespace {anonymous}

BOOST_AUTO_TEST_SUITE(TestRegistrarCertification)
BOOST_FIXTURE_TEST_SUITE(Update, test_update_certification_fixture)

BOOST_AUTO_TEST_CASE(update_registrar_certification_date)
{
    LibFred::OperationContextCreator ctx;
    LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx);
    Database::Result result = ctx.get_conn().exec_params(
            "SELECT * FROM registrar_certification "
            "WHERE id = $1::bigint",
            Database::query_param_list(certification_id));
    BOOST_REQUIRE_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(test_registrar.id, static_cast<unsigned long long>(result[0]["registrar_id"]));
    BOOST_CHECK(valid_from == boost::gregorian::from_string(static_cast<std::string>(result[0]["valid_from"])));
    BOOST_CHECK(valid_until == boost::gregorian::from_string(static_cast<std::string>(result[0]["valid_until"])));
    BOOST_CHECK_EQUAL(score, static_cast<int>(result[0]["classification"]));
    BOOST_CHECK_EQUAL(file_id, static_cast<unsigned long long>(result[0]["eval_file_id"]));

    valid_until -= boost::gregorian::date_duration(1);
    LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx);
    Database::Result new_result = ctx.get_conn().exec_params(
            "SELECT * FROM registrar_certification "
            "WHERE id = $1::bigint",
            Database::query_param_list(certification_id));
    BOOST_REQUIRE_EQUAL(new_result.size(), 1);
    BOOST_CHECK_EQUAL(test_registrar.id, static_cast<unsigned long long>(new_result[0]["registrar_id"]));
    BOOST_CHECK(valid_from == boost::gregorian::from_string(static_cast<std::string>(new_result[0]["valid_from"])));
    BOOST_CHECK(valid_until == boost::gregorian::from_string(static_cast<std::string>(new_result[0]["valid_until"])));
    BOOST_CHECK_EQUAL(score, static_cast<int>(new_result[0]["classification"]));
    BOOST_CHECK_EQUAL(file_id, static_cast<unsigned long long>(new_result[0]["eval_file_id"]));
}

BOOST_AUTO_TEST_CASE(update_registrar_certification_score_file)
{
    LibFred::OperationContextCreator ctx;
    const int new_score = 2;
    LibFred::Registrar::UpdateRegistrarCertification{certification_id}
            .set_classification(new_score)
            .set_eval_file_id(file_uuid).exec(ctx);
    Database::Result result = ctx.get_conn().exec_params(
            "SELECT registrar_id, valid_from, classification, eval_file_id FROM registrar_certification "
            "WHERE id = $1::bigint "
            "AND valid_until IS NULL",
            Database::query_param_list(certification_id));
    BOOST_REQUIRE_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(test_registrar.id, static_cast<unsigned long long>(result[0]["registrar_id"]));
    BOOST_CHECK(valid_from == boost::gregorian::from_string(static_cast<std::string>(result[0]["valid_from"])));
    BOOST_CHECK_EQUAL(new_score, static_cast<int>(result[0]["classification"]));
    BOOST_CHECK_EQUAL(file_id, static_cast<unsigned long long>(result[0]["eval_file_id"]));
}

BOOST_AUTO_TEST_CASE(certification_in_past)
{
    LibFred::OperationContextCreator ctx;
    valid_until -= boost::gregorian::date_duration(3);
    BOOST_CHECK_THROW(
        LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx),
        CertificationInPast);
}

BOOST_AUTO_TEST_CASE(invalid_date_until)
{
    LibFred::OperationContextCreator ctx;
    LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx);
    valid_until = boost::gregorian::date(not_a_date_time);
    BOOST_CHECK_THROW(
        LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx),
        InvalidDateTo);
}

BOOST_AUTO_TEST_CASE(wrong_interval_order)
{
    LibFred::OperationContextCreator ctx;
    valid_until -= boost::gregorian::date_duration(2);
    BOOST_CHECK_THROW(
        LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_valid_until(valid_until).exec(ctx),
        WrongIntervalOrder);
}

BOOST_AUTO_TEST_CASE(score_overcome)
{
    LibFred::OperationContextCreator ctx;
    const int score = 6;
    BOOST_CHECK_THROW(
        LibFred::Registrar::UpdateRegistrarCertification{certification_id}.set_classification(score).exec(ctx),
        ScoreOutOfRange);
}

BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertification/Update
BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertification
