/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

#include <iostream>

namespace Util {

std::ostream& operator<<(std::ostream& out, CaseInsensitive::ComparisonResult value)
{
    switch (value)
    {
        case ::Util::CaseInsensitive::ComparisonResult::equal:
            return out << "equal";
        case ::Util::CaseInsensitive::ComparisonResult::not_equal:
            return out << "not equal";
    }
    return out << "???";
}

}//namespace Util

BOOST_AUTO_TEST_SUITE(Tests)
BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(CaseInsensitive)

BOOST_AUTO_TEST_CASE(test_comparison)
{
    const std::string lower_case = "příliš žluťoučký kůň úpěl ďábelské ódy";
    const std::string upper_case = "PŘÍLIŠ ŽLUŤOUČKÝ KŮŇ ÚPĚL ĎÁBELSKÉ ÓDY";
    BOOST_CHECK_NE(::Util::CaseInsensitive::ComparisonResult::equal, ::Util::CaseInsensitive::ComparisonResult::not_equal);
    BOOST_CHECK_NE(lower_case, upper_case);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(lower_case, lower_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(lower_case, upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(upper_case, lower_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(upper_case, upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(upper_case + " ", upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::not_equal);
    LibFred::OperationContextCreator ctx;
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(ctx.get_conn(), lower_case, lower_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(ctx.get_conn(), lower_case, upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(ctx.get_conn(), upper_case, lower_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(ctx.get_conn(), upper_case, upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::equal);
    BOOST_CHECK_EQUAL(::Util::CaseInsensitive::compare(ctx.get_conn(), upper_case + " ", upper_case),
                      ::Util::CaseInsensitive::ComparisonResult::not_equal);
};

BOOST_AUTO_TEST_SUITE_END()//Tests/Util/CaseInsensitive
BOOST_AUTO_TEST_SUITE_END()//Tests/Util
BOOST_AUTO_TEST_SUITE_END()//Tests
