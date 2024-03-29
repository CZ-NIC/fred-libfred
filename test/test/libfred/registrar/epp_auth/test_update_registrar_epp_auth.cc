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

#include "libfred/registrar/epp_auth/exceptions.hh"
#include "libfred/registrar/epp_auth/update_registrar_epp_auth.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/libfred/registrar/epp_auth/util.hh"
#include "test/libfred/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/optional.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <limits>
#include <string>

namespace Test{

namespace {

constexpr const char* default_certificate = "00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F";
constexpr const char* default_password = "pass_xxx";

struct UpdateRegistrarEppAuthFixture : has_registrar
{
    UpdateRegistrarEppAuthFixture()
        : registrar_handle(registrar.handle),
          certificate_fingerprint(default_certificate),
          plain_password(default_password),
          id(add_epp_authentications(ctx, registrar_handle, certificate_fingerprint, plain_password, std::string{}).id)
    { }
    std::string& registrar_handle;
    std::string certificate_fingerprint;
    std::string plain_password;
    unsigned long long id;
    static const std::string no_cert_data;
};

const std::string UpdateRegistrarEppAuthFixture::no_cert_data;

} //namespace Test::{anonymous}

BOOST_FIXTURE_TEST_SUITE(TestUpdateRegistrarEppAuth, UpdateRegistrarEppAuthFixture)

BOOST_AUTO_TEST_CASE(set_no_update_data)
{
    BOOST_CHECK_THROW(
            ::LibFred::Registrar::EppAuth::UpdateRegistrarEppAuth(id).exec(ctx),
            ::LibFred::Registrar::EppAuth::NoUpdateData);
}

BOOST_AUTO_TEST_CASE(set_nonexistent_registrar_epp_auth)
{
    id = Random::Generator().get(
            std::numeric_limits<unsigned>::min(), 
            std::numeric_limits<unsigned>::max());
    BOOST_CHECK_THROW(
            ::LibFred::Registrar::EppAuth::UpdateRegistrarEppAuth(id)
            .set_plain_password(plain_password)
            .exec(ctx),
            ::LibFred::Registrar::EppAuth::NonexistentRegistrarEppAuth);
}

BOOST_AUTO_TEST_CASE(set_update_registrar_certificate_fingerprint)
{
    const std::string new_certificate = get_random_fingerprint();
    ::LibFred::Registrar::EppAuth::UpdateRegistrarEppAuth(id)
            .set_certificate(new_certificate, no_cert_data)
            .set_plain_password(boost::none)
            .exec(ctx);
    BOOST_CHECK_EQUAL(get_epp_auth_id(ctx, registrar_handle, new_certificate, default_password), id);
}

BOOST_AUTO_TEST_CASE(set_update_registrar_plain_password)
{
    const std::string new_password = Random::Generator().get_seq(Random::CharSet::letters_and_digits(), 10);
    ::LibFred::Registrar::EppAuth::UpdateRegistrarEppAuth(id)
            .set_plain_password(new_password)
            .exec(ctx);
    BOOST_CHECK_EQUAL(get_epp_auth_id(ctx, registrar_handle, default_certificate, new_password), id);
}

BOOST_AUTO_TEST_CASE(set_update_registrar_all_epp_auth)
{
    const std::string new_certificate = get_random_fingerprint();
    const std::string new_password = Random::Generator().get_seq(Random::CharSet::letters_and_digits(), 10);
    ::LibFred::Registrar::EppAuth::UpdateRegistrarEppAuth(id)
            .set_certificate(new_certificate, no_cert_data)
            .set_plain_password(new_password)
            .exec(ctx);
    BOOST_CHECK_EQUAL(get_epp_auth_id(ctx, registrar_handle, new_certificate, new_password), id);
}

BOOST_AUTO_TEST_SUITE_END()//TestUpdateRegistrarEppAuth

} //namespace Test
