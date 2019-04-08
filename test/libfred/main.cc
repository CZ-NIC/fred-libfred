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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFredlib

#include "config.h"

#include "test/setup/fixtures.hh"

#include "test/fake-src/util/cfg/config_handler_decl.hh"
#include "test/fake-src/util/cfg/config_handler.hh"

#include "test/fake-src/util/cfg/handle_tests_args.hh"
#include "test/fake-src/util/cfg/handle_logging_args.hh"
#include "test/fake-src/util/cfg/handle_database_args.hh"

// dynamic library version
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include <utility>

namespace Test {

#ifndef CONFIG_FILE
constexpr const char* CONFIG_FILE = "libfred.conf";
#endif

struct handle_command_line_args
{
    handle_command_line_args()
    {
        config_handlers = boost::assign::list_of
                (HandleArgsPtr(new HandleTestsArgs(CONFIG_FILE)))
                (HandleArgsPtr(new HandleLoggingArgs))
                (HandleArgsPtr(new HandleDatabaseArgs))
                (HandleArgsPtr(new HandleAdminDatabaseArgs)).convert_to_container<HandlerPtrVector>();

        CfgArgs::init<HandleTestsArgs>(config_handlers)->handle(
                boost::unit_test::framework::master_test_suite().argc,
                boost::unit_test::framework::master_test_suite().argv).copy_onlynospaces_args();
    }
    HandlerPtrVector config_handlers;
};

void setup_logging(CfgArgs* cfg_instance_ptr)
{
    const HandleLoggingArgs* const handler_ptr = cfg_instance_ptr->get_handler_ptr_by_type<HandleLoggingArgs>();

    const auto log_device = static_cast<Logging::Log::Device>(handler_ptr->log_type);

    switch (log_device)
    {
        case Logging::Log::Device::file:
            Logging::Manager::instance_ref().add_handler_of<Logging::Log::Device::file>(
                    static_cast<std::string>(handler_ptr->log_file),
                    static_cast<Logging::Log::EventImportance>(handler_ptr->log_level));
            break;
        case Logging::Log::Device::syslog:
            Logging::Manager::instance_ref().add_handler_of<Logging::Log::Device::syslog>(
                    static_cast<int>(handler_ptr->log_syslog_facility),
                    static_cast<Logging::Log::EventImportance>(handler_ptr->log_level));
            break;
        case Logging::Log::Device::console:
            Logging::Manager::instance_ref().add_handler_of<Logging::Log::Device::console>(
                    static_cast<Logging::Log::EventImportance>(handler_ptr->log_level));
            break;
    }
}

}//namespace Test

struct global_fixture
{
    Test::handle_command_line_args handle_admin_db_cmd_line_args;
    Test::create_db_template crete_db_template;

    global_fixture()
    {
        Test::setup_logging(CfgArgs::instance());
    }
};

BOOST_GLOBAL_FIXTURE(global_fixture);
