/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/zone/create_zone.hh"
#include "libfred/zone/exceptions.hh"
#include "libfred/zone/zone_soa/create_zone_soa.hh"
#include "libfred/zone/zone_soa/default_values.hh"
#include "libfred/zone/zone_soa/exceptions.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"

#include "test/libfred/zone/zone_soa/util.hh"
#include "test/libfred/zone/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

namespace Test {

struct CreateZoneSoaFixture
{
    CreateZoneSoaFixture(const LibFred::OperationContext& _ctx)
        : fqdn(Random::Generator().get_seq(Random::CharSet::letters(), 3)),
          hostmaster("hostmaster@nic.cz"),
          ns_fqdn("a.ns.nic." + fqdn)
    {
        ::LibFred::Zone::CreateZone(fqdn, 5, 6).exec(_ctx);
    }
    ~CreateZoneSoaFixture() {}
    std::string fqdn;
    std::string hostmaster;
    std::string ns_fqdn;
};

BOOST_FIXTURE_TEST_SUITE(TestCreateZoneSoa, SupplyFixtureCtx<CreateZoneSoaFixture>)

unsigned long long get_zone_soa_id(
        const ::LibFred::OperationContext& _ctx,
        const std::string& _fqdn,
        const std::string& _hostmaster,
        const std::string& _ns_fqdn,
        const unsigned long _ttl = ::LibFred::Zone::default_ttl_in_seconds,
        const unsigned long _refresh = ::LibFred::Zone::default_refresh_in_seconds,
        const unsigned long _update_retr = ::LibFred::Zone::default_update_retr_in_seconds,
        const unsigned long _expiry = ::LibFred::Zone::default_expiry_in_seconds,
        const unsigned long _minimum = ::LibFred::Zone::default_minimum_in_seconds)
{
    const Database::Result db_result = _ctx.get_conn().exec_params(
            "SELECT zs.zone "
            "FROM zone z "
            "JOIN zone_soa zs ON zs.zone=z.id "
            "WHERE z.fqdn=LOWER($1::text) AND "
                  "zs.hostmaster=$2::text AND "
                  "zs.ns_fqdn=$3::text AND "
                  "zs.ttl=$4::bigint AND "
                  "zs.refresh=$5::bigint AND "
                  "zs.update_retr=$6::bigint AND "
                  "zs.expiry=$7::bigint AND "
                  "zs.minimum=$8::bigint",
            Database::query_param_list(_fqdn)
                                    (_hostmaster)
                                    (_ns_fqdn)
                                    (_ttl)
                                    (_refresh)
                                    (_update_retr)
                                    (_expiry)
                                    (_minimum));
    if (db_result.size() == 1)
    {
        return static_cast<unsigned long long>(db_result[0][0]);
    }
    throw std::runtime_error("unexpected number of records in the zone_soa table");
}

BOOST_AUTO_TEST_CASE(set_nonexistent_zone)
{
    BOOST_CHECK_THROW(::LibFred::Zone::CreateZoneSoa(Random::Generator().get_seq(Random::CharSet::letters(), 3), hostmaster, ns_fqdn)
                .exec(ctx),
           ::LibFred::Zone::NonExistentZone);
}

BOOST_AUTO_TEST_CASE(set_existing_zone_soa)
{
    ::LibFred::Zone::CreateZoneSoa(fqdn, hostmaster, ns_fqdn).exec(ctx);
    BOOST_CHECK_THROW(::LibFred::Zone::CreateZoneSoa(fqdn, hostmaster, ns_fqdn)
                .exec(ctx),
            ::LibFred::Zone::AlreadyExistingZoneSoa);
}

BOOST_AUTO_TEST_CASE(set_min_create_zone_soa)
{
    const unsigned long long id = ::LibFred::Zone::CreateZoneSoa(fqdn, hostmaster, ns_fqdn).exec(ctx);
    BOOST_CHECK_EQUAL(get_zone_soa_id(ctx, fqdn, hostmaster, ns_fqdn), id);
}

BOOST_AUTO_TEST_CASE(set_max_create_zone_soa)
{
    const unsigned long long id = ::LibFred::Zone::CreateZoneSoa(fqdn, hostmaster, ns_fqdn)
           .set_ttl(new_ttl_in_seconds)
           .set_refresh(new_refresh_in_seconds)
           .set_update_retr(new_update_retr_in_seconds)
           .set_expiry(new_expiry_in_seconds)
           .set_minimum(new_minimum_in_seconds)
           .exec(ctx);
    BOOST_CHECK_EQUAL(
            get_zone_soa_id(ctx, fqdn, hostmaster, ns_fqdn, new_ttl_in_seconds, new_refresh_in_seconds,
                            new_update_retr_in_seconds, new_expiry_in_seconds, new_minimum_in_seconds),
            id);
}

BOOST_AUTO_TEST_SUITE_END()//TestCreateZoneSoa

}//namespace Test
