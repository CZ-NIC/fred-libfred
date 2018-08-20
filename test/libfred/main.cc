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

    const Logging::Log::Type log_type = static_cast<Logging::Log::Type>(handler_ptr->log_type);

    boost::any param;
    if (log_type == Logging::Log::LT_FILE)
    {
        param = handler_ptr->log_file;
    }
    if (log_type == Logging::Log::LT_SYSLOG)
    {
        param = handler_ptr->log_syslog_facility;
    }

    Logging::Manager::instance_ref().get(PACKAGE).addHandler(log_type, param);

    Logging::Manager::instance_ref().get(PACKAGE).setLevel(static_cast<Logging::Log::Level>(handler_ptr->log_level));
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
