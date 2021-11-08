/*
 * Copyright (C) 2021  CZ.NIC, z. s. p. o.
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

#include "libfred/opcontext.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrar/get_registrar_handles.hh"

#include "test/libfred/util.hh"

#include "test/fake-src/util/cfg/config_handler_decl.hh"
#include "test/fake-src/util/cfg/handle_database_args.hh"

#include "libpg/pg_ro_transaction.hh"
#include "libpg/pg_rw_transaction.hh"

#include <boost/test/unit_test.hpp>

#include <string>
#include <utility>

BOOST_FIXTURE_TEST_SUITE(TestOperationContext, Test::instantiate_db_template)

namespace {

auto get_dsn()
{
    const auto* const cfg_ptr = CfgArgs::instance()->get_handler_ptr_by_type<HandleDatabaseArgs>();
    LibPg::Dsn dsn;
    if (cfg_ptr->has_connect_timeout())
    {
        dsn.connect_timeout = LibPg::Dsn::ConnectTimeout{std::chrono::seconds{cfg_ptr->get_connect_timeout_sec()}};
    }
    dsn.db_name = LibPg::Dsn::DbName{cfg_ptr->get_db_name()};
    dsn.host = LibPg::Dsn::Host{cfg_ptr->get_host()};
    if (cfg_ptr->has_password())
    {
        dsn.password = LibPg::Dsn::Password{cfg_ptr->get_password()};
    }
    if (cfg_ptr->has_port())
    {
        dsn.port = LibPg::Dsn::Port{static_cast<std::uint16_t>(cfg_ptr->get_port())};
    }
    dsn.user = LibPg::Dsn::User{cfg_ptr->get_user()};
    return dsn;
}

auto get_registrar(const LibFred::OperationContext& ctx)
{
    return LibFred::Registrar::GetRegistrarHandles{}.exec(ctx).front();
}

}//namespace {anonymous}

BOOST_AUTO_TEST_CASE(use_operation_context_by_libpg)
{
    FREDLOG_SET_CONTEXT(LogCtx, log_ctx, "use_operation_context_by_libpg");
    auto ro_tx = LibPg::PgRoTransaction{get_dsn()};
    const auto registrar = get_registrar(ro_tx);
    auto rw_tx = LibPg::PgRwTransaction{commit(std::move(ro_tx))};
    static constexpr const char contact_handle[] = "TEST-CONTACT-LIBPG";
    static constexpr const char contact_name[] = "Libuše Post Gresí";
    LibFred::CreateContact{contact_handle, registrar}.set_name(contact_name).exec(rw_tx);
    const auto contact = LibFred::InfoContactByHandle{contact_handle}.exec(rw_tx).info_contact_data;
    commit(std::move(rw_tx));
    BOOST_CHECK_EQUAL(contact.name.get_value(), contact_name);
}

BOOST_AUTO_TEST_SUITE_END()//TestOperationContext
