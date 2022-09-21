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

#include "test/setup/fixtures.hh"

#include "test/fake-src/util/cfg/config_handler_decl.hh"
#include "test/fake-src/util/cfg/handle_database_args.hh"

#include "src/libfred/db_settings.hh"
#include "src/libfred/registrable_object/domain/domain_name.hh"
#include "src/libfred/registrar/info_registrar.hh"
#include "src/libfred/zone/create_zone.hh"
#include "src/libfred/zone/exceptions.hh"
#include "src/libfred/zone/info_zone.hh"

#include "util/log/log.hh"

#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/domain/create_domain.hh"
#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrar/create_registrar.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/zone/create_zone.hh"
#include "libfred/zone/exceptions.hh"
#include "libfred/zone/info_zone.hh"

/** well, these includes are ugly
 * but there is no other way to get to the name of current test_case
 * needed for ~instantiate_db_template() to store post-test db copy
 */
#include <boost/test/framework.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <stdexcept>
#include <utility>

namespace Test {

namespace po = boost::program_options;

namespace {

constexpr unsigned max_postgresql_database_name_length = 63;

void check_dbname_length(const std::string& db_name)
{
    if (max_postgresql_database_name_length < db_name.length())
    {
        throw std::runtime_error{
                "db_name.length(): " + std::to_string(db_name.length()) + " > "
                "max_postgresql_database_name_length: " + std::to_string(max_postgresql_database_name_length) + " "
                "db_name: " + db_name};
    }
}

void drop_db(const std::string& db_name, const std::unique_ptr<Database::StandaloneConnection>& conn)
{
    check_dbname_length(db_name);
    conn->exec("DROP DATABASE IF EXISTS \""+ db_name +"\"");
}

void copy_db(
        const std::string& src_name,
        const std::string& dst_name,
        const std::unique_ptr<Database::StandaloneConnection>& conn)
{
    check_dbname_length(src_name);
    check_dbname_length(dst_name);
    drop_db(dst_name, conn);
    conn->exec("CREATE DATABASE \"" + dst_name + "\" TEMPLATE \"" + src_name + "\"");
}

decltype(auto) make_registrar(LibFred::OperationContext& ctx, LibFred::CreateRegistrar& create)
{
    try
    {
        ctx.get_conn().exec("SAVEPOINT make_registrar");
        const auto registrar_id = create.exec(ctx);
        ctx.get_conn().exec("RELEASE SAVEPOINT make_registrar");
        return LibFred::InfoRegistrarById{registrar_id}.exec(ctx).info_registrar_data;
    }
    catch (const LibFred::CreateRegistrar::Exception& e)
    {
        if (e.is_set_invalid_registrar_handle())
        {
            ctx.get_conn().exec("ROLLBACK TO SAVEPOINT make_registrar");
            return LibFred::InfoRegistrarByHandle{e.get_invalid_registrar_handle()}.exec(ctx).info_registrar_data;
        }
        throw;
    }
}

std::string get_version(int index)
{
   if (index < 0)
   {
       throw std::runtime_error{"negative index is not allowed"};
   }
   static constexpr char number_of_letters = 'Z' - 'A';
   if (index <= number_of_letters)
   {
       return std::string(1, 'A' + index);
   }
   return std::to_string(index - number_of_letters);
}

void set_registrar(LibFred::CreateRegistrar& op, bool system, int index)
{
    const auto version = get_version(index);
    if (system)
   {
       op.set_name(version + " System Registrar")
         .set_organization(version + " System Registrar Ltd")
         .set_email("system-registrar-" + version + "@nic.com")
         .set_url("https://system-registrar-" + version + ".nic.com");
   }
   else
   {
       op.set_name(version + " Registrar")
         .set_organization(version + " Registrar Gmbh.")
         .set_email("registrar-" + version + "@registrar.com")
         .set_url("https://registrar-" + version + ".registrar.com");
   }
   op.set_street1("Street 1 - " + version)
     .set_street2("Street 2 - " + version)
     .set_street3("Street 3 - " + version)
     .set_city("City " + version)
     .set_stateorprovince("State Or Province " + version)
     .set_postalcode("143 21")
     .set_country("CZ")
     .set_telephone("+420.441207848")
     .set_fax("+420.361971091")
     .set_ico("1234" + version)
     .set_dic("5678" + version)
     .set_variable_symbol("VS-" + std::to_string(2 * index + (system ? 1 : 0)))
     .set_payment_memo_regex(version)
     .set_vat_payer(index % 2 != 0);
}

struct GetNonEnumZone : boost::static_visitor<LibFred::Zone::NonEnumZone>
{
    LibFred::Zone::NonEnumZone operator()(const LibFred::Zone::NonEnumZone& value) const
    {
        return value;
    }
    template <typename T>
    LibFred::Zone::NonEnumZone operator()(const T&) const
    {
        throw std::runtime_error{"not a non-enum zone"};
    }
};

struct GetEnumZone : boost::static_visitor<LibFred::Zone::EnumZone>
{
    LibFred::Zone::EnumZone operator()(const LibFred::Zone::EnumZone& value) const
    {
        return value;
    }
    template <typename T>
    LibFred::Zone::EnumZone operator()(const T&) const
    {
        throw std::runtime_error{"not an enum zone"};
    }
};

auto get_non_enum_zone(LibFred::OperationContext& ctx, const char* zone, bool idn_enabled)
{
    try
    {
        return boost::apply_visitor(GetNonEnumZone{}, LibFred::Zone::InfoZone{zone}.exec(ctx));
    }
    catch (const LibFred::Zone::NonExistentZone&) { }
    catch (const LibFred::Zone::InfoZoneException& e)
    {
        FREDLOG_INFO(boost::format{"InfoZone failed: %1%"} % e.what());
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(boost::format{"InfoZone failed: %1%"} % e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_ERROR("InfoZone failed by an unexpected exception");
        throw;
    }
    static constexpr int expiration_period_min_in_months = 12;
    static constexpr int expiration_period_max_in_months = 120;
    LibFred::Zone::CreateZone{zone, expiration_period_min_in_months, expiration_period_max_in_months}
            .exec(ctx);
    if (!idn_enabled)
    {
        LibFred::Domain::set_domain_name_validation_config_into_database(ctx, zone, { "dncheck_no_consecutive_hyphens" });
    }
    return boost::apply_visitor(GetNonEnumZone{}, LibFred::Zone::InfoZone{zone}.exec(ctx));
}

auto get_enum_zone(LibFred::OperationContext& ctx, const char* zone, int enum_validation_period_in_months)
{
    try
    {
        return boost::apply_visitor(GetEnumZone{}, LibFred::Zone::InfoZone{zone}.exec(ctx));
    }
    catch (const LibFred::Zone::NonExistentZone&) { }
    catch (const LibFred::Zone::InfoZoneException& e)
    {
        FREDLOG_INFO(boost::format{"InfoZone failed: %1%"} % e.what());
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(boost::format{"InfoZone failed: %1%"} % e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_ERROR("InfoZone failed by an unexpected exception");
        throw;
    }
    static constexpr int expiration_period_min_in_months = 12;
    static constexpr int expiration_period_max_in_months = 120;
    LibFred::Zone::CreateZone{zone, expiration_period_min_in_months, expiration_period_max_in_months}
            .set_enum_validation_period_in_months(enum_validation_period_in_months)
            .exec(ctx);
    LibFred::Domain::set_domain_name_validation_config_into_database(ctx, zone, { "dncheck_single_digit_labels_only" });
    return boost::apply_visitor(GetEnumZone{}, LibFred::Zone::InfoZone{zone}.exec(ctx));
}

std::string make_fqdn(const char* subdomain, std::size_t subdomain_length, const char* zone)
{
    auto fqdn = std::string{};
    if (subdomain_length == 0)
    {
        fqdn = zone;
        return fqdn;
    }
    const auto zone_length = std::strlen(zone);
    if (subdomain[subdomain_length - 1] == '.')
    {
        fqdn.reserve(subdomain_length + zone_length);
        fqdn.append(subdomain, subdomain_length);
    }
    else
    {
        fqdn.reserve(subdomain_length + 1 + zone_length);
        fqdn.append(subdomain, subdomain_length);
        fqdn.append(1, '.');
    }
    fqdn.append(zone, zone_length);
    return fqdn;
}

std::string make_fqdn(const char* subdomain, const char* zone)
{
    return make_fqdn(subdomain, std::strlen(subdomain), zone);
}

std::string make_fqdn(const std::string& subdomain, const char* zone)
{
    return make_fqdn(subdomain.c_str(), subdomain.length(), zone);
}

std::string make_fqdn(unsigned long long subdomain, const char* zone)
{
    auto fqdn = std::string{};
    if (subdomain == 0)
    {
        fqdn = zone;
        return fqdn;
    }
    const auto number_of_digits = [](unsigned long long value)
    {
        unsigned result = 1;
        while (10 <= value)
        {
            ++result;
            value /= 10;
        }
        return result;
    }(subdomain);
    const auto zone_length = std::strlen(zone);
    fqdn.reserve(2 * number_of_digits + zone_length);
    while (0 < subdomain)
    {
        const char level[2] = {char('0' + (subdomain % 10)), '.'};
        fqdn.append(level, 2);
        subdomain /= 10;
    }
    fqdn.append(zone, zone_length);
    return fqdn;
}

}//namespace Test::{anonymous}

std::string get_original_db_name()
{
    return CfgArgs::instance()->get_handler_ptr_by_type<HandleDatabaseArgs>()->get_db_name();
}

std::string create_db_template::get_db_template_name()
{
    return get_original_db_name() + "_test_template";
}

create_db_template::create_db_template()
{
    check_dbname_length(get_original_db_name());
    check_dbname_length(get_db_template_name());
    copy_db(get_original_db_name(),
            get_db_template_name(),
            CfgArgs::instance()->get_handler_ptr_by_type<HandleAdminDatabaseArgs>()->get_admin_connection());
}

create_db_template::~create_db_template()
{
    // restore original db for the last time
    copy_db(get_db_template_name(),
            get_original_db_name(),
            CfgArgs::instance()->get_handler_ptr_by_type<HandleAdminDatabaseArgs>()->get_admin_connection());
    drop_db(get_db_template_name(),
            CfgArgs::instance()->get_handler_ptr_by_type<HandleAdminDatabaseArgs>()->get_admin_connection());
}

namespace {

std::string get_unit_test_path(const boost::unit_test::test_unit& tu,
                               const std::string& delimiter = "/")
{
    using namespace boost::unit_test;
    const bool is_orphan = (tu.p_parent_id < MIN_TEST_SUITE_ID) || (tu.p_parent_id == INV_TEST_UNIT_ID);
    if (is_orphan)
    {
        return static_cast<std::string>(tu.p_name);
    }
    return get_unit_test_path(framework::get<test_suite>(tu.p_parent_id), delimiter) +
           delimiter + static_cast<std::string>(tu.p_name);
}

}//namespace Test::{anonymous}

std::string instantiate_db_template::testcase_db_name()
{
    const std::string db_name =
            get_original_db_name() + "_" +
            get_unit_test_path(boost::unit_test::framework::current_test_case(), "_") +
            db_name_suffix_;
    if (db_name.length() <= max_postgresql_database_name_length)
    {
        return db_name;
    }
    return db_name.substr(db_name.length() - max_postgresql_database_name_length,
                          max_postgresql_database_name_length);
}

instantiate_db_template::instantiate_db_template(const std::string& db_name_suffix)
    : db_name_suffix_(db_name_suffix)
{
    BOOST_REQUIRE(testcase_db_name().length() <= max_postgresql_database_name_length);

    copy_db(create_db_template::get_db_template_name(),
            get_original_db_name(),
            CfgArgs::instance()->get_handler_ptr_by_type<HandleAdminDatabaseArgs>()->get_admin_connection());
}

instantiate_db_template::~instantiate_db_template()
{
    const auto log_db_name = testcase_db_name();
    const auto conn =
            CfgArgs::instance()->get_handler_ptr_by_type<HandleAdminDatabaseArgs>()->get_admin_connection();

    drop_db(log_db_name, conn);
    conn->exec("ALTER DATABASE \"" + get_original_db_name() + "\" RENAME TO \"" + log_db_name + "\"");
}

std::shared_ptr<po::options_description> HandleAdminDatabaseArgs::get_options_description()
{
     auto db_opts = std::make_shared<po::options_description>("Admin database connection configuration");

     db_opts->add_options()
         ("admin_database.name",
          po::value<std::string>()->default_value("fred"),
          "admin database name")
         ("admin_database.user",
          po::value<std::string>()->default_value("fred"),
          "admin database user name")
         ("admin_database.password",
          po::value<std::string>()->default_value("password"),
          "admin database password")
         ("admin_database.host",
          po::value<std::string>()->default_value("localhost"),
          "admin database hostname")
         ("admin_database.port",
          po::value<unsigned int>()->default_value(5432),
          "admin database port number")
         ("admin_database.timeout",
          po::value<unsigned int>()->default_value(10),
          "admin database timeout");

     return db_opts;
}

void HandleAdminDatabaseArgs::handle(int argc, char* argv[], FakedArgs& fa)
{
    po::variables_map vm;

    handler_parse_args()(get_options_description(), vm, argc, argv, fa);

    /* construct connection string */
    host = vm["admin_database.host"].as<std::string>();
    pass = vm["admin_database.password"].as<std::string>();
    dbname = vm["admin_database.name"].as<std::string>();
    user = vm["admin_database.user"].as<std::string>();
    port = std::to_string(vm["admin_database.port"].as<unsigned>());
    timeout = std::to_string(vm["admin_database.timeout"].as<unsigned>());
}

std::unique_ptr<Database::StandaloneConnection> HandleAdminDatabaseArgs::get_admin_connection()
{
    return Database::StandaloneManager(
            "host=" + host + " "
            "port=" + port + " "
            "dbname=" + dbname + " "
            "user=" + user + " "
            "password=" + pass + " "
            "connect_timeout=" + timeout).acquire();
}

Registrar::Registrar(LibFred::OperationContext& ctx, LibFred::CreateRegistrar create)
    : data{make_registrar(ctx, create)}
{ }

SystemRegistrar::SystemRegistrar(LibFred::OperationContext& ctx, LibFred::CreateRegistrar create)
    : data{make_registrar(ctx, create.set_system(true))}
{ }

Zone::Zone(LibFred::OperationContext& ctx, const char* zone, bool idn_enabled)
    : data{get_non_enum_zone(ctx, zone, idn_enabled)}
{ }

EnumZone::EnumZone(LibFred::OperationContext& ctx, const char* zone, int enum_validation_period_in_months)
    : data{get_enum_zone(ctx, zone, enum_validation_period_in_months)}
{ }

CzZone::CzZone(LibFred::OperationContext& ctx)
    : Zone{ctx, fqdn()}
{ }

const char* CzZone::fqdn() noexcept { return "cz"; }

std::string CzZone::fqdn(const char* subdomain)
{
    return make_fqdn(subdomain, fqdn());
}

std::string CzZone::fqdn(const std::string& subdomain)
{
    return make_fqdn(subdomain, fqdn());
}

constexpr int default_enum_validation_period_in_months = 6;

CzEnumZone::CzEnumZone(LibFred::OperationContext& ctx)
    : EnumZone{ctx, fqdn(), default_enum_validation_period_in_months}
{ }

const char* CzEnumZone::fqdn() noexcept { return "0.2.4.e164.arpa"; }

std::string CzEnumZone::fqdn(unsigned long long subdomain)
{
    return make_fqdn(subdomain, fqdn());
}

InitDomainNameCheckers::InitDomainNameCheckers(LibFred::OperationContext& ctx)
{
    ctx.get_conn().exec("SAVEPOINT init_domain_name_checkers");
    try
    {
        LibFred::Domain::insert_domain_name_checker_name_into_database(ctx, "dncheck_no_consecutive_hyphens", "forbid consecutive hyphens");
        ctx.get_conn().exec("RELEASE SAVEPOINT init_domain_name_checkers");
    }
    catch (...)
    {
        ctx.get_conn().exec("ROLLBACK TO SAVEPOINT init_domain_name_checkers");
    }
    ctx.get_conn().exec("SAVEPOINT init_domain_name_checkers");
    try
    {
        LibFred::Domain::insert_domain_name_checker_name_into_database(ctx, "dncheck_single_digit_labels_only", "enforces single digit labels (for enum domains)");
        ctx.get_conn().exec("RELEASE SAVEPOINT init_domain_name_checkers");
    }
    catch (...)
    {
        ctx.get_conn().exec("ROLLBACK TO SAVEPOINT init_domain_name_checkers");
    }
}

HasOperationContext::~HasOperationContext()
{
    try
    {
        ctx.commit_transaction();
    }
    catch (...) { }
}

namespace {

auto make_registrar(LibFred::OperationContext& ctx, const std::string& handle, bool is_system, bool is_internal)
{
    try
    {
        return LibFred::InfoRegistrarByHandle{handle}.exec(ctx).info_registrar_data.id;
    }
    catch (const LibFred::InfoRegistrarByHandle::Exception& e)
    {
        if (!e.is_set_unknown_registrar_handle())
        {
            throw;
        }
        return LibFred::CreateRegistrar{handle}
                .set_name("Registrar " + handle)
                .set_system(is_system)
                .set_internal(is_internal)
                .exec(ctx);
    }
}

}//namespace Test::{anonymous}

HasRegistrar::HasRegistrar(LibFred::OperationContext& ctx, std::string handle, bool is_system, bool is_internal)
    : id{make_registrar(ctx, handle, is_system, is_internal)},
      handle{std::move(handle)}
{ }

namespace {

auto make_zone(LibFred::OperationContext& ctx, const std::string& fqdn)
{
    try
    {
        return get_zone_id(LibFred::Zone::InfoZone{fqdn}.exec(ctx));
    }
    catch (const LibFred::Zone::NonExistentZone& e)
    {
        return LibFred::Zone::CreateZone{fqdn, 12, 120}.exec(ctx);
    }
}

}//namespace Test::{anonymous}

HasZone::HasZone(LibFred::OperationContext& ctx, std::string fqdn)
    : id{make_zone(ctx, fqdn)},
      fqdn{std::move(fqdn)}
{ }

namespace {

auto make_contact(LibFred::OperationContext& ctx, const std::string& handle, const HasRegistrar& registrar)
{
    try
    {
        return LibFred::InfoContactByHandle{handle}.exec(ctx).info_contact_data.id;
    }
    catch (const LibFred::InfoContactByHandle::Exception& e)
    {
        if (!e.is_set_unknown_contact_handle())
        {
            throw;
        }
        return LibFred::CreateContact{handle, registrar.handle}
                .set_name("Contact " + handle)
                .exec(ctx).create_object_result.object_id;
    }
}

}//namespace Test::{anonymous}

HasContact::HasContact(LibFred::OperationContext& ctx, std::string handle, const HasRegistrar& registrar)
    : id{make_contact(ctx, handle, registrar)},
      handle{std::move(handle)}
{ }

namespace {

auto make_domain(
        LibFred::OperationContext& ctx,
        const std::string& fqdn,
        const HasRegistrar& registrar,
        const HasContact& registrant)
{
    try
    {
        return LibFred::InfoDomainByFqdn{fqdn}.exec(ctx).info_domain_data.id;
    }
    catch (const LibFred::InfoDomainByFqdn::Exception& e)
    {
        if (!e.is_set_unknown_fqdn())
        {
            throw;
        }
        return LibFred::CreateDomain{fqdn, registrar.handle, registrant.handle}.exec(ctx).create_object_result.object_id;
    }
}

}//namespace Test::{anonymous}

HasDomain::HasDomain(
        LibFred::OperationContext& ctx,
        std::string fqdn,
        const HasRegistrar& registrar,
        const HasContact& registrant)
    : id{make_domain(ctx, fqdn, registrar, registrant)},
      fqdn{std::move(fqdn)}
{ }

}//namespace Test

using namespace Test::Setter;

LibFred::CreateRegistrar Test::Setter::registrar(LibFred::CreateRegistrar create, int index)
{
    set_registrar(create, false, index);
    return create;
}

LibFred::CreateRegistrar Test::Setter::system_registrar(LibFred::CreateRegistrar create, int index)
{
    set_registrar(create, true, index);
    return create;
}
