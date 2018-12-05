/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/zone/create_zone.hh"
#include "libfred/zone/exceptions.hh"
#include "libfred/zone/zone_soa/create_zone_soa.hh"
#include "libfred/zone/zone_soa/exceptions.hh"
#include "util/random_data_generator.hh"
#include "test/libfred/zone/zone_soa/util.hh"
#include "test/libfred/zone/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

namespace Test {

namespace {

struct CreateZoneSoaFixture
{
    CreateZoneSoaFixture(LibFred::OperationContext& _ctx)
        : fqdn(RandomDataGenerator().xstring(3))
    {
        ::LibFred::Zone::CreateZone(fqdn, 5, 6).exec(_ctx);
    }
    std::string fqdn;
};

bool does_new_zone_soa_exist(LibFred::OperationContext& _ctx, const std::string& _fqdn)
{
    return 0 < _ctx.get_conn().exec_params(
            "SELECT 0 "
            "FROM zone z "
            "JOIN zone_soa zs ON zs.zone=z.id "
            "WHERE z.fqdn=LOWER($1::text) "
            "LIMIT 1",
            Database::query_param_list(_fqdn)).size();
}

}//namespace Test::{anonymous}

BOOST_FIXTURE_TEST_SUITE(TestCreateZoneSoa, SupplyFixtureCtx<CreateZoneSoaFixture>)

BOOST_AUTO_TEST_CASE(set_nonexistent_zone)
{
    BOOST_CHECK_THROW(::LibFred::Zone::CreateZoneSoa(RandomDataGenerator().xstring(3)).exec(ctx),
                      ::LibFred::Zone::NonExistentZone);
}

BOOST_AUTO_TEST_CASE(set_existing_zone_soa)
{
    ::LibFred::Zone::CreateZoneSoa(fqdn).exec(ctx);
    BOOST_CHECK_THROW(::LibFred::Zone::CreateZoneSoa(fqdn).exec(ctx),
                      ::LibFred::Zone::AlreadyExistingZoneSoa);
}

BOOST_AUTO_TEST_CASE(set_min_create_zone_soa)
{
    ::LibFred::Zone::CreateZoneSoa(fqdn).exec(ctx);
    BOOST_CHECK(does_new_zone_soa_exist(ctx, fqdn));
}

BOOST_AUTO_TEST_CASE(set_max_create_zone_soa)
{
    ::LibFred::Zone::CreateZoneSoa(fqdn)
           .set_ttl(new_ttl_in_seconds)
           .set_hostmaster(new_hostmaster)
           .set_refresh(new_refresh_in_seconds)
           .set_update_retr(new_update_retr_in_seconds)
           .set_expiry(new_expiry_in_seconds)
           .set_minimum(new_minimum_in_seconds)
           .set_ns_fqdn(new_ns_fqdn)
           .exec(ctx);
    BOOST_CHECK(does_new_zone_soa_exist(ctx, fqdn));
}

BOOST_AUTO_TEST_SUITE_END()//TestCreateZoneSoa

}//namespace Test
