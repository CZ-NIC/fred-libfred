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

#include <boost/test/unit_test.hpp>

#include "libfred/object/check_authinfo.hh"
#include "libfred/object/clean_expired_authinfos.hh"
#include "libfred/object/store_authinfo.hh"

#include "test/setup/fixtures.hh"

namespace {

struct HasBasicObjects
    : Test::instantiate_db_template,
      Test::HasOperationContext,
      Test::HasZone      
{
    HasBasicObjects()
        : Test::instantiate_db_template{},
          Test::HasOperationContext{},
          Test::HasZone{ctx, "cz"},
          registrar{ctx, "REG-TEST-1", false, false},
          registrar2{ctx, "REG-TEST-2", false, false},
          contact{ctx, "CONTACT-TEST", registrar},
          domain{ctx, "domain-test.cz", registrar, contact}
    { }
    Test::HasRegistrar registrar;
    Test::HasRegistrar registrar2;
    Test::HasContact contact;
    Test::HasDomain domain;
};

}//namespace {anonymous}

BOOST_AUTO_TEST_SUITE(TestRegistryObjectAuthinfo)

BOOST_FIXTURE_TEST_CASE(store_authinfo_happy, HasBasicObjects)
{
    const auto first_id = LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar.id,
            std::chrono::seconds{3600}}.exec(ctx, "password");
    BOOST_CHECK_LT(0, *first_id);
    const auto second_id = LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar.id,
            std::chrono::seconds{3600}}.exec(ctx, "another password");
    BOOST_CHECK_LT(*first_id, *second_id);
}

BOOST_FIXTURE_TEST_CASE(store_authinfo_exception, HasBasicObjects)
{
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::StoreAuthinfo(
                    LibFred::Object::ObjectId{123456789ull},
                    registrar.id,
                    std::chrono::seconds{3600}).exec(ctx, "password"),
            LibFred::UnknownObjectId,
            [](const LibFred::UnknownObjectId&) { return true; });
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::StoreAuthinfo(
                    LibFred::Object::ObjectId{domain.id},
                    123456789ull,
                    std::chrono::seconds{3600}).exec(ctx, "password"),
            LibFred::UnknownRegistrar,
            [](const LibFred::UnknownRegistrar&) { return true; });
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::StoreAuthinfo(
                    LibFred::Object::ObjectId{domain.id},
                    registrar.id,
                    std::chrono::seconds{0}).exec(ctx, "password"),
            LibFred::Object::InvalidTtl,
            [](const LibFred::Object::InvalidTtl&) { return true; });
}

BOOST_FIXTURE_TEST_CASE(clean_expired_authinfos_happy, HasBasicObjects)
{
    BOOST_CHECK_EQUAL(LibFred::Object::CleanExpiredAuthinfos{}.exec(ctx), 0);

    const auto first_id = LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar.id,
            std::chrono::seconds{3600}}.exec(ctx, "password");
    BOOST_CHECK_LT(0, *first_id);
    const auto second_id = LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{contact.id},
            registrar.id,
            std::chrono::seconds{3600}}.exec(ctx, "another password");
    BOOST_CHECK_LT(*first_id, *second_id);
    BOOST_REQUIRE_EQUAL(
            ctx.get_conn().exec_params(
                    "UPDATE object_authinfo "
                       "SET created_at = created_at - '2HOURS'::INTERVAL, "
                           "expires_at = expires_at - '2HOURS'::INTERVAL "
                     "WHERE id IN ($1::INT, $2::INT) "
                 "RETURNING id",
                    Database::QueryParams{first_id, second_id}).size(),
            2);
    BOOST_CHECK_EQUAL(LibFred::Object::CleanExpiredAuthinfos{}.exec(ctx), 2);
}

BOOST_FIXTURE_TEST_CASE(check_authinfo_happy, HasBasicObjects)
{
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            0);
    LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar.id,
            std::chrono::seconds{3600}}.exec(ctx, "password");
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "bad password", LibFred::Object::CheckAuthinfo::increment_usage),
            0);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{contact.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            0);
    LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar2.id,
            std::chrono::seconds{3600}}.exec(ctx, "password");
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            2);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "another password", LibFred::Object::CheckAuthinfo::increment_usage),
            0);
    LibFred::Object::StoreAuthinfo{
            LibFred::Object::ObjectId{domain.id},
            registrar2.id,
            std::chrono::seconds{3600}}.exec(ctx, "another password");
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "another password", LibFred::Object::CheckAuthinfo::increment_usage),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "another password", LibFred::Object::CheckAuthinfo::increment_usage_and_cancel),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "another password", LibFred::Object::CheckAuthinfo::increment_usage_and_cancel),
            0);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage_and_cancel),
            1);
    BOOST_CHECK_EQUAL(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{domain.id}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage_and_cancel),
            0);
}

BOOST_FIXTURE_TEST_CASE(check_authinfo_exception, HasBasicObjects)
{
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{123456789ull}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage),
            LibFred::UnknownObjectId,
            [](const LibFred::UnknownObjectId&) { return true; });
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{123456789ull}}
                    .exec(ctx, "password", LibFred::Object::CheckAuthinfo::increment_usage_and_cancel),
            LibFred::UnknownObjectId,
            [](const LibFred::UnknownObjectId&) { return true; });
    static const LibFred::Object::CheckAuthinfo::Visitor throw_exception =
            [](const LibFred::OperationContext&, const LibFred::Object::AuthinfoId&)
            {
                struct Exception : std::exception
                {
                    const char* what() const noexcept override { return __PRETTY_FUNCTION__; }
                };
                throw Exception{};
            };
    BOOST_CHECK_EXCEPTION(
            LibFred::Object::CheckAuthinfo{LibFred::Object::ObjectId{123456789ull}}
                    .exec(ctx, "password", throw_exception),
            LibFred::UnknownObjectId,
            [](const LibFred::UnknownObjectId&) { return true; });
}

BOOST_AUTO_TEST_SUITE_END()//TestRegistryObjectAuthinfo
