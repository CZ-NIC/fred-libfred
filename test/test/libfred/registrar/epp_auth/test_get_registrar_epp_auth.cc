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

#include "libfred/registrar/epp_auth/get_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"

#include "test/libfred/registrar/epp_auth/util.hh"
#include "test/libfred/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

namespace Test {

struct GetRegistrarEppAuthFixture : has_registrar
{
    std::string& registrar_handle;
    std::string certificate_fingerprint;
    std::string plain_password;

    GetRegistrarEppAuthFixture()
        : registrar_handle(registrar.handle),
          certificate_fingerprint(get_random_fingerprint()),
          plain_password(Random::Generator().get_seq(Random::CharSet::letters(), 10))
    {
    }
};

BOOST_FIXTURE_TEST_SUITE(TestGetRegistrarEppAuth, GetRegistrarEppAuthFixture)

BOOST_AUTO_TEST_CASE(set_nonexistent_registrar)
{
    const std::string nonexistent_registrar = Random::Generator().get_seq(Random::CharSet::letters(), 10);
    ::LibFred::Registrar::EppAuth::RegistrarEppAuthData epp_auth_data =
            ::LibFred::Registrar::EppAuth::GetRegistrarEppAuth(nonexistent_registrar).exec(ctx);
    BOOST_CHECK_EQUAL(epp_auth_data.epp_auth_records.size(), 0);
}

BOOST_AUTO_TEST_CASE(set_nonexistent_registrar_epp_auth)
{
    ::LibFred::Registrar::EppAuth::RegistrarEppAuthData epp_auth_data =
            ::LibFred::Registrar::EppAuth::GetRegistrarEppAuth(registrar_handle).exec(ctx);
    BOOST_CHECK_EQUAL(epp_auth_data.epp_auth_records.size(), 0);
}

BOOST_AUTO_TEST_CASE(set_registrar_epp_authentication)
{
    add_epp_authentications(ctx, registrar_handle, certificate_fingerprint, plain_password, std::string{});

    ::LibFred::Registrar::EppAuth::RegistrarEppAuthData epp_auth_data =
            ::LibFred::Registrar::EppAuth::GetRegistrarEppAuth(registrar_handle).exec(ctx);

    for (const auto& epp_auth : epp_auth_data.epp_auth_records)
    {
        BOOST_REQUIRE_LT(0, get_epp_auth_id(ctx, registrar_handle, epp_auth.certificate_fingerprint));
    }
}

BOOST_AUTO_TEST_SUITE_END()//TestGetRegistrarEppAuth

} // namespace Test
