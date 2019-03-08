/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
#include "libfred/registrable_object/contact/get_contact_data_history.hh"
#include "libfred/registrable_object/contact/get_contact_handle_history.hh"
#include "util/printable.hh"
#include "libfred/registrable_object/uuid.hh"
#include "libfred/object/object_type.hh"

#include <boost/test/unit_test.hpp>

#include <array>
#include <iostream>
#include <string>

BOOST_AUTO_TEST_SUITE(TestContactHistory)

struct MyFixture
{
};

using TimePointConverter = SqlConvert<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>>;

BOOST_FIXTURE_TEST_CASE(contact_data_history, MyFixture)
{
    using namespace LibFred::RegistrableObject::Contact;
    LibFred::OperationContextCreator ctx;
    const auto interval = LibFred::RegistrableObject::HistoryInterval(
            LibFred::RegistrableObject::HistoryInterval::LowerLimit(
//                            TimePointConverter::from("2018-08-29 13:42:41.478909")),
                    LibFred::RegistrableObject::HistoryInterval::NoLimit()),
            LibFred::RegistrableObject::HistoryInterval::UpperLimit(
//                            TimePointConverter::from("2018-08-29 13:42:41.907719"))));
                    LibFred::RegistrableObject::HistoryInterval::NoLimit()));

    const auto data1_history = GetContactDataHistoryById(1).exec(ctx, interval);
    for (const auto& record : data1_history.timeline)
    {
        std::cout << TimePointConverter::to(record.valid_from) << std::endl;
    }
    if (data1_history.valid_to != boost::none)
    {
        std::cout << TimePointConverter::to(*data1_history.valid_to) << std::endl;
    }
    const auto uuid =
            LibFred::RegistrableObject::make_uuid_of<LibFred::Object_Type::contact>(
                    "5aa575e9-089f-4d05-a5fc-98171fba93b9");
    const auto handle = "KONTAKT";
    GetContactDataHistoryByUuid(uuid).exec(ctx, interval);
    GetContactDataHistoryByHandle(handle).exec(ctx, interval);
}

BOOST_AUTO_TEST_SUITE_END()//TestContactHistory
