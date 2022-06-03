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
/**
 *  @file
 *  test fixtures
 */

#ifndef FIXTURES_HH_327B2D873D064624803561323E8E3BF3
#define FIXTURES_HH_327B2D873D064624803561323E8E3BF3

#include "test/fake-src/util/cfg/handle_args.hh"

#include "libfred/db_settings.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrar/create_registrar.hh"
#include "libfred/registrar/info_registrar_data.hh"
#include "libfred/zone/info_zone_data.hh"

#include <string>

/**
 * @file fixtures for data isolation in tests
 * for more info see documentation with examples:
 * https://admin.nic.cz/wiki/developers/fred/tests
 */

namespace Test {

// database created by fred-manager init_cz
std::string get_original_db_name();

struct create_db_template
{
    static std::string get_db_template_name();
    create_db_template();
    virtual ~create_db_template();
};

struct instantiate_db_template
{
    const std::string db_name_suffix_;/**< suffix of the name of database instance left in database cluster after fixture teardown, useful in case of more database instances per testcase */
    instantiate_db_template(const std::string& db_name_suffix = "");
    virtual ~instantiate_db_template();
private:
    std::string testcase_db_name();
};


/***
 * config handlers for admin connection to db used by fixtures related to db data
 */
class HandleAdminDatabaseArgs : public HandleArgs
{
    public:
        std::string host;
        std::string port;
        std::string user;
        std::string pass;
        std::string dbname;
        std::string timeout;

        std::shared_ptr<boost::program_options::options_description> get_options_description();

        void handle( int argc, char* argv[],  FakedArgs &fa);

        std::unique_ptr<Database::StandaloneConnection> get_admin_connection();
};

struct Registrar
{
    explicit Registrar(LibFred::OperationContext& ctx, LibFred::CreateRegistrar create);
    LibFred::InfoRegistrarData data;
};

struct SystemRegistrar
{
    explicit SystemRegistrar(LibFred::OperationContext& ctx, LibFred::CreateRegistrar create);
    LibFred::InfoRegistrarData data;
};

struct Zone
{
    explicit Zone(
            LibFred::OperationContext& ctx,
            const char* zone,
            bool idn_enabled = false);
    LibFred::Zone::NonEnumZone data;
};

struct EnumZone
{
    explicit EnumZone(
            LibFred::OperationContext& ctx,
            const char* zone,
            int enum_validation_period_in_months);
    LibFred::Zone::EnumZone data;
};

struct CzZone : Zone
{
    explicit CzZone(LibFred::OperationContext& ctx);
    static const char* fqdn() noexcept;
    static std::string fqdn(const char* subdomain);
    static std::string fqdn(const std::string& subdomain);
};

struct CzEnumZone : EnumZone
{
    explicit CzEnumZone(LibFred::OperationContext& ctx);
    static const char* fqdn() noexcept;
    static std::string fqdn(unsigned long long subdomain);
};

struct InitDomainNameCheckers
{
    explicit InitDomainNameCheckers(LibFred::OperationContext& ctx);
};

namespace Setter {

LibFred::CreateRegistrar registrar(LibFred::CreateRegistrar create, int index = 0);
LibFred::CreateRegistrar system_registrar(LibFred::CreateRegistrar create, int index = 0);

}//namespace Test::Setter

}//namespace Test

#endif//FIXTURES_HH_327B2D873D064624803561323E8E3BF3
