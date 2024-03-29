/*
 * Copyright (C) 2020-2022  CZ.NIC, z. s. p. o.
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

#include "libfred/registrar/epp_auth/add_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/exceptions.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/libfred/registrar/epp_auth/util.hh"
#include "test/libfred/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

namespace {

struct AddRegistrarEppAuthFixture : Test::has_registrar
{
    AddRegistrarEppAuthFixture()
        : Test::has_registrar{},
          certificate_fingerprint{Test::get_random_fingerprint()},
          plain_password{Random::Generator().get_seq(Random::CharSet::letters(), 10)}
    { }
    auto add_registrar_epp_auth()
    {
        return ::LibFred::Registrar::EppAuth::AddRegistrarEppAuth(
                    registrar.handle, certificate_fingerprint, plain_password, no_cert_data).exec(ctx);
    }
    std::string certificate_fingerprint;
    std::string plain_password;
    static const std::string no_cert_data;
};

const std::string AddRegistrarEppAuthFixture::no_cert_data;

} // namespace {anonymous}

namespace Test {

BOOST_FIXTURE_TEST_SUITE(TestAddRegistrarEppAuth, AddRegistrarEppAuthFixture)

BOOST_AUTO_TEST_CASE(set_nonexistent_registrar)
{
    registrar.handle = Random::Generator().get_seq(Random::CharSet::letters(), 20);
    BOOST_CHECK_THROW(add_registrar_epp_auth(), ::LibFred::Registrar::EppAuth::NonexistentRegistrar);
}

BOOST_AUTO_TEST_CASE(set_duplicate_eep_auth)
{
    add_registrar_epp_auth();
    BOOST_CHECK_THROW(add_registrar_epp_auth(), ::LibFred::Registrar::EppAuth::DuplicateCertificate);
}

BOOST_AUTO_TEST_CASE(set_add_registrar_epp_auth)
{
    const auto id = add_registrar_epp_auth();
    BOOST_CHECK_EQUAL(get_epp_auth_id(ctx, registrar.handle, certificate_fingerprint, plain_password), id);
}

BOOST_AUTO_TEST_SUITE_END()//TestAddRegistrarEppAuth

} // namespace Test
