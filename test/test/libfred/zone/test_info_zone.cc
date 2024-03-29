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
#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/zone/create_zone.hh"
#include "libfred/zone/info_zone_data.hh"
#include "libfred/zone/info_zone.hh"
#include "libfred/zone/exceptions.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/libfred/zone/util.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

namespace Test {

struct info_zone_fixture
{
    ::LibFred::Zone::NonEnumZone non_enum_zone;
    ::LibFred::Zone::EnumZone enum_zone;

    info_zone_fixture(const ::LibFred::OperationContext& _ctx)
    {
        non_enum_zone.fqdn = "zoo";
        non_enum_zone.expiration_period_max_in_months = 6;
        non_enum_zone.expiration_period_min_in_months = 8;
        non_enum_zone.dots_max = 1;
        non_enum_zone.sending_warning_letter = false;

        non_enum_zone.id = ::LibFred::Zone::CreateZone(
                non_enum_zone.fqdn,
                non_enum_zone.expiration_period_min_in_months,
                non_enum_zone.expiration_period_max_in_months)
                                   .exec(_ctx);

        enum_zone.fqdn = "3.2.1.e164.arpa";
        enum_zone.expiration_period_max_in_months = 12;
        enum_zone.expiration_period_min_in_months = 24;
        enum_zone.validation_period_in_months = 4;
        enum_zone.dots_max = 9;
        enum_zone.sending_warning_letter = true;

        enum_zone.id = ::LibFred::Zone::CreateZone(enum_zone.fqdn,
                enum_zone.expiration_period_min_in_months,
                enum_zone.expiration_period_max_in_months)
                               .set_enum_validation_period_in_months(enum_zone.validation_period_in_months)
                               .set_sending_warning_letter(enum_zone.sending_warning_letter)
                               .exec(_ctx);
    }

    ~info_zone_fixture()
    {}
};

BOOST_FIXTURE_TEST_SUITE(TestInfoZone, SupplyFixtureCtx<info_zone_fixture>)

BOOST_AUTO_TEST_CASE(set_nonexistent_zone)
{
    std::string fqdn = Random::Generator().get_seq(Random::CharSet::letters(), 5);
    BOOST_CHECK_THROW(::LibFred::Zone::InfoZone(fqdn)
                .exec(ctx),
           ::LibFred::Zone::NonExistentZone);
}

BOOST_AUTO_TEST_CASE(set_info_zone)
{
    ::LibFred::Zone::InfoZoneData zone_info = ::LibFred::Zone::InfoZone(non_enum_zone.fqdn).exec(ctx);
    BOOST_CHECK(non_enum_zone == boost::get<::LibFred::Zone::NonEnumZone>(zone_info));
}

BOOST_AUTO_TEST_CASE(set_info_enum_zone)
{
    ::LibFred::Zone::InfoZoneData zone_info = ::LibFred::Zone::InfoZone(enum_zone.fqdn).exec(ctx);
    BOOST_CHECK(enum_zone == boost::get<::LibFred::Zone::EnumZone>(zone_info));
}

BOOST_AUTO_TEST_SUITE_END();

} // namespace Test
