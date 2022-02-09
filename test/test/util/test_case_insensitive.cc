/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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
#include "src/util/case_insensitive.hh"

#include "src/libfred/opcontext.hh"

#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_AUTO_TEST_SUITE(Tests)
BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(CaseInsensitive)

BOOST_FIXTURE_TEST_CASE(test_comparison, Test::instantiate_db_template)
{
    const std::string lower_case = "příliš žluťoučký kůň úpěl ďábelské ódy";
    const std::string upper_case = "PŘÍLIŠ ŽLUŤOUČKÝ KŮŇ ÚPĚL ĎÁBELSKÉ ÓDY";
    BOOST_CHECK_NE(lower_case, upper_case);
    BOOST_CHECK(::Util::case_insensitive_equal_to()(lower_case, lower_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to()(lower_case, upper_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to()(upper_case, lower_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to()(upper_case, upper_case));
    BOOST_CHECK(!::Util::case_insensitive_equal_to()(upper_case + " ", upper_case));
    LibFred::OperationContextCreator ctx;
    BOOST_CHECK(::Util::case_insensitive_equal_to(ctx.get_conn())(lower_case, lower_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to(ctx.get_conn())(lower_case, upper_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to(ctx.get_conn())(upper_case, lower_case));
    BOOST_CHECK(::Util::case_insensitive_equal_to(ctx.get_conn())(upper_case, upper_case));
    BOOST_CHECK(!::Util::case_insensitive_equal_to(ctx.get_conn())(upper_case + " ", upper_case));
};

BOOST_AUTO_TEST_SUITE_END()//Tests/Util/CaseInsensitive
BOOST_AUTO_TEST_SUITE_END()//Tests/Util
BOOST_AUTO_TEST_SUITE_END()//Tests
