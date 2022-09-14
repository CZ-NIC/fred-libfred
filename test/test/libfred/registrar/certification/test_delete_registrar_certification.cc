/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/certification/delete_registrar_certification.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"
#include "libfred/opcontext.hh"

#include <boost/test/unit_test.hpp>

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

struct test_delete_certification_fixture : virtual public Test::instantiate_db_template
{
    test_delete_certification_fixture()
    {
        LibFred::OperationContextCreator ctx;
        const auto test_registrar = Test::registrar::make(ctx);
        const auto valid_from = boost::gregorian::day_clock::local_day() + boost::gregorian::date_duration(1);
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
             "RETURNING uuid");
        const auto file_uuid = LibFred::Registrar::FileUuid{to_uuid(dbres[0][0])};
        constexpr auto score = 1;
        certification_data = LibFred::Registrar::create_registrar_certification(
                ctx,
                test_registrar.id,
                valid_from,
                boost::gregorian::date{boost::date_time::special_values::pos_infin},
                score,
                file_uuid);
        ctx.commit_transaction();
    }
    LibFred::Registrar::RegistrarCertification certification_data;
};

}//namespace {anonymous}

BOOST_AUTO_TEST_SUITE(TestRegistrarCertification)
BOOST_FIXTURE_TEST_SUITE(Delete, test_delete_certification_fixture)

BOOST_AUTO_TEST_CASE(delete_by_id)
{
    LibFred::OperationContextCreator ctx;
    const auto result = LibFred::Registrar::delete_registrar_certification(ctx, certification_data.id);
    BOOST_CHECK(result == certification_data);
}

BOOST_AUTO_TEST_CASE(delete_by_uuid)
{
    LibFred::OperationContextCreator ctx;
    const auto result = LibFred::Registrar::delete_registrar_certification(ctx, certification_data.uuid);
    BOOST_CHECK(result == certification_data);
}

BOOST_AUTO_TEST_CASE(delete_by_non_existing_id)
{
    LibFred::OperationContextCreator ctx;
    BOOST_CHECK_THROW(
        LibFred::Registrar::DeleteRegistrarCertification{0}.exec(ctx),
        RegistrarCertificationDoesNotExist);
}

BOOST_AUTO_TEST_CASE(delete_by_non_existing_uuid)
{
    LibFred::OperationContextCreator ctx;
    BOOST_CHECK_THROW(
        LibFred::Registrar::DeleteRegistrarCertification{LibFred::Registrar::RegistrarCertificationUuid{}}.exec(ctx),
        RegistrarCertificationDoesNotExist);
}

BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertification/Delete
BOOST_AUTO_TEST_SUITE_END()//TestRegistrarCertification
