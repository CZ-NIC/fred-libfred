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
/**
 *  @file
 *  functions for listing of various auxiliary enumerated objects realted to admin contact verification
 */

#ifndef LIST_ENUM_OBJECTS_HH_091937DEDDFA47F6806AC5068E1EB3B2
#define LIST_ENUM_OBJECTS_HH_091937DEDDFA47F6806AC5068E1EB3B2

#include "util/printable.hh"

#include <vector>
#include <string>

namespace LibFred
{
    struct test_result_status {
        std::string handle;
        std::string name;
        std::string description;

        test_result_status(const std::string _handle, const std::string _name, const std::string _description)
            : handle(_handle), name(_name), description(_description)
        { }
    };
    std::vector<test_result_status> list_test_result_statuses(const std::string& lang );


    struct check_status {
        std::string handle;
        std::string name;
        std::string description;

        check_status(const std::string _handle, const std::string _name, const std::string _description)
            : handle(_handle), name(_name), description(_description)
        { }
    };
    std::vector<check_status> list_check_statuses(const std::string& lang );


    struct test_definition {
        std::string handle;
        std::string name;
        std::string description;

        test_definition(const std::string _handle, const std::string _name, const std::string _description)
            : handle(_handle), name(_name), description(_description)
        { }
    };
    std::vector<test_definition> list_test_definitions(const std::string& lang, const std::string& testsuite_name = "" );


    struct testsuite_definition {
        std::string handle;
        std::string name;
        std::string description;
        std::vector<test_definition> tests;

        testsuite_definition(const std::string _handle, const std::string _name, const std::string _description)
            : handle(_handle), name(_name), description(_description)
        { }
    };
    std::vector<testsuite_definition> list_testsuite_definitions(const std::string& lang );
}
#endif
