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

#include "test/setup/cfg.hh"
#include "test/setup/fixtures.hh"

#include "test/fake-src/util/cfg/config_handler.hh"

#include "test/fake-src/util/cfg/handle_tests_args.hh"
#include "test/fake-src/util/cfg/handle_logging_args.hh"
#include "test/fake-src/util/cfg/handle_database_args.hh"

#include "src/libfred/opcontext.hh"

#include "src/util/log/add_log_device.hh"

// dynamic library version
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include <stdexcept>
#include <utility>

namespace {

void database_setup()
{
    LibFred::OperationContextCreator ctx;
    Test::CzZone{ctx};
    Test::CzEnumZone{ctx};
    Test::InitDomainNameCheckers{ctx};
    Test::SystemRegistrar{ctx, Test::Setter::system_registrar(LibFred::CreateRegistrar{"REG-CZNIC"})};
    Test::Registrar{ctx, Test::Setter::registrar(LibFred::CreateRegistrar{"REG-MOJEID"}, 1)};
    ctx.get_conn().exec(
            "WITH u1 AS ("
                "UPDATE enum_parameters SET val = '2' WHERE name = 'handle_registration_protection_period' "
                "RETURNING id) "
            "UPDATE enum_parameters SET val = 'CZ' WHERE name = 'roid_suffix'");
//    Test::Registrar{ctx, Test::Setter::registrar(LibFred::CreateRegistrar{"REG-A"}, 1)};
//    Test::Registrar{ctx, Test::Setter::registrar(LibFred::CreateRegistrar{"REG-B"}, 2)};
//    Test::Registrar{ctx, Test::Setter::registrar(LibFred::CreateRegistrar{"REG-C"}, 3)};
    ctx.commit_transaction();
}

class GlobalFixture
{
public:
    GlobalFixture()
        : database_administrator_{
                []()
                {
                    const HandlerPtrVector config_handlers = {
                            std::make_shared<HandleLoggingArgs>(),
                            std::make_shared<HandleDatabaseArgs>(),
                            std::make_shared<Test::HandleAdminDatabaseArgs>()};
                    return Test::Cfg::handle_command_line_args(config_handlers, database_setup);
                }()}
    {
        FREDLOG_INFO("tests start");
    }
    ~GlobalFixture()
    {
        try
        {
            FREDLOG_INFO("tests done");
        }
        catch (...) { }
    }
private:
    Test::Cfg::DatabaseAdministrator database_administrator_;
    Test::create_db_template create_db_template;
};

}//namespace {anonymous}

BOOST_GLOBAL_FIXTURE(GlobalFixture);
