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
#include "libfred/registrar/certification/get_registrar_certifications.hh"
#include "libfred/registrar/certification/exceptions.hh"
#include "libfred/registrar/certification/registrar_certification_type.hh"

#include "libfred/db_settings.hh"
#include "libfred/opcontext.hh"
#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"

#include <boost/test/unit_test.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <algorithm>

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

struct test_get_certifications_fixture : virtual public Test::instantiate_db_template
{
    test_get_certifications_fixture()
    {
        LibFred::OperationContextCreator ctx;
        test_registrar = Test::registrar::make(ctx);
        const auto file_uuid = LibFred::Registrar::FileUuid{to_uuid(ctx.get_conn().exec(
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
             "RETURNING uuid")[0][0])};
        unsigned score = 1;
        unsigned date_duration = 10;
        LibFred::Registrar::RegistrarCertification rc;
        rc.valid_from = boost::gregorian::day_clock::local_day();
        for (unsigned int i = 0; i < certifications_amount; ++i)
        {
            rc.valid_from += boost::gregorian::date_duration(date_duration);
            if (i == certifications_amount - 1)
            {
                rc.valid_until = boost::gregorian::date(boost::gregorian::pos_infin);
            }
            else
            {
                rc.valid_until = rc.valid_from + boost::gregorian::date_duration(date_duration - 1);
            }
            const auto record = LibFred::Registrar::create_registrar_certification(
                    ctx,
                    test_registrar.id,
                    rc.valid_from,
                    boost::gregorian::date{boost::date_time::special_values::pos_infin},
                    score, file_uuid);
            rc.id = record.id;
            rc.uuid = record.uuid;
            rc.eval_file_id = record.eval_file_id;
            rc.eval_file_uuid = record.eval_file_uuid;
            rc.classification = record.classification;
            reg_certs.push_back(rc);
            ++score;
        }
        ctx.commit_transaction();
        std::reverse(reg_certs.begin(), reg_certs.end());
    }
    LibFred::InfoRegistrarData test_registrar;
    static const unsigned int certifications_amount;
    std::vector<LibFred::Registrar::RegistrarCertification> reg_certs;
};

const unsigned int test_get_certifications_fixture::certifications_amount = 3;

}//namespace {anonymous}

BOOST_AUTO_TEST_SUITE(TestRegistrarCertifications)
BOOST_FIXTURE_TEST_SUITE(Get, test_get_certifications_fixture)

BOOST_AUTO_TEST_CASE(get_registrar_certifications)
{
    LibFred::OperationContextCreator ctx;
    std::vector<LibFred::Registrar::RegistrarCertification> result =
            LibFred::Registrar::GetRegistrarCertifications(test_registrar.id).exec(ctx);

    BOOST_CHECK(result.size() == certifications_amount);
    for (unsigned int i = 0; i < certifications_amount; ++i)
    {
        BOOST_CHECK(reg_certs.at(i) == result.at(i));
    }
}

BOOST_AUTO_TEST_CASE(registrar_not_found)
{
    LibFred::OperationContextCreator ctx;
    const Database::Result reg_id = ctx.get_conn().exec(
            "SELECT nextval('registrar_id_seq'::regclass)");
    BOOST_CHECK_THROW(
        LibFred::Registrar::GetRegistrarCertifications(static_cast<int>(reg_id[0][0]) + 1).exec(ctx),
        RegistrarNotFound);
}

BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertifications/Get
BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertifications
