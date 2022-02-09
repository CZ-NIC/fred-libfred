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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFredlib

#include "config.h"

#include "test/setup/fixtures.hh"

#include "test/fake-src/util/cfg/config_handler_decl.hh"
#include "test/fake-src/util/cfg/config_handler.hh"

#include "test/fake-src/util/cfg/handle_tests_args.hh"
#include "test/fake-src/util/cfg/handle_logging_args.hh"
#include "test/fake-src/util/cfg/handle_database_args.hh"

#include "liblog/log.hh"
#include "liblog/sink/console_sink_config.hh"
#include "liblog/sink/file_sink_config.hh"
#include "liblog/sink/syslog_sink_config.hh"

// dynamic library version
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include <stdexcept>
#include <utility>

namespace Test {

#ifndef CONFIG_FILE
constexpr const char* CONFIG_FILE = "libfred.conf";
#endif

namespace {

enum class LogSeverity
{
    emerg,
    alert,
    crit,
    err,
    warning,
    notice,
    info,
    debug,
    trace
};

constexpr LibLog::Level severity_to_level(LogSeverity severity)
{
    switch (severity)
    {
        case LogSeverity::emerg:
            return LibLog::Level::critical;
        case LogSeverity::alert:
            return LibLog::Level::critical;
        case LogSeverity::crit:
            return LibLog::Level::critical;
        case LogSeverity::err:
            return LibLog::Level::error;
        case LogSeverity::warning:
            return LibLog::Level::warning;
        case LogSeverity::notice:
            return LibLog::Level::info;
        case LogSeverity::info:
            return LibLog::Level::info;
        case LogSeverity::debug:
            return LibLog::Level::debug;
        case LogSeverity::trace:
            return LibLog::Level::trace;
    }
    throw std::runtime_error{"unexpected log severity"};
}

auto make_console_config(LogSeverity severity)
{
    LibLog::Sink::ConsoleSinkConfig config;
    config.set_level(severity_to_level(severity));
    config.set_output_stream(LibLog::Sink::ConsoleSinkConfig::OutputStream::stderr);
    config.set_color_mode(LibLog::ColorMode::never);
    return config;
}

auto make_file_config(LogSeverity severity, const std::string& file)
{
    LibLog::Sink::FileSinkConfig config{file};
    config.set_level(severity_to_level(severity));
    return config;
}

auto make_syslog_config(LogSeverity severity, unsigned facility)
{
    LibLog::Sink::SyslogSinkConfig config;
    config.set_level(severity_to_level(severity));
    config.set_syslog_facility(static_cast<int>(facility));
    return config;
}

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
        setup_logging(CfgArgs::instance());
    }
    HandlerPtrVector config_handlers;

    static void setup_logging(CfgArgs* cfg_instance_ptr)
    {
        const HandleLoggingArgs* const handler_ptr = cfg_instance_ptr->get_handler_ptr_by_type<HandleLoggingArgs>();

        LibLog::LogConfig log_config;
        const auto has_log_device = [&]()
        {
            switch (handler_ptr->log_type)
            {
                case 0:
                    log_config.add_sink_config(make_console_config(static_cast<LogSeverity>(handler_ptr->log_level)));
                    return true;
                case 1:
                    log_config.add_sink_config(make_file_config(static_cast<LogSeverity>(handler_ptr->log_level), handler_ptr->log_file));
                    return true;
                case 2:
                    log_config.add_sink_config(make_syslog_config(static_cast<LogSeverity>(handler_ptr->log_level), handler_ptr->log_syslog_facility));
                    return true;
            }
            return false;
        }();
        if (has_log_device)
        {
            LibLog::Log::start<LibLog::ThreadMode::multi_threaded>(log_config);
        }
    }

};

}//namespace Test::{anonymous}
}//namespace Test

struct global_fixture
{
    Test::handle_command_line_args handle_admin_db_cmd_line_args;
    Test::create_db_template crete_db_template;
};

BOOST_GLOBAL_FIXTURE(global_fixture);
