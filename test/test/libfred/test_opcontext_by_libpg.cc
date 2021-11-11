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
#include "libpg/detail/libpq_layer_impl.hh"

#include <boost/test/framework.hpp>
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

std::string serialize_connectdb_params(
        const char* const* keywords,
        const char* const* values)
{
    std::string result;
    while (*keywords != nullptr)
    {
        if (!result.empty())
        {
            result.append(" ");
        }
        static const std::string keyword_password = "password";
        const auto& keyword = *keywords;
        result.append(keyword).append(":");
        if (keyword == keyword_password)
        {
            result.append("******");
        }
        else
        {
            const auto& value = *values;
            result.append(value != nullptr ? value : "");
        }
        ++keywords;
        ++values;
    }
    return result;
}

std::string value_to_string(const char* value)
{
    if (value == nullptr)
    {
        return "NULL";
    }
    static constexpr auto quotation_mark = "`";
    return std::string{quotation_mark}.append(value).append(quotation_mark);
}

std::string serialize_exec_params(
        int n_params,
        const ::Oid* param_types,
        const char* const* param_values,
        const int* param_lengths,
        const int* param_formats)
{
    std::string result;
    const bool all_params_are_string = (param_types == nullptr) &&
                                       (param_lengths == nullptr) &&
                                       (param_formats == nullptr);
    if (all_params_are_string)
    {
        int param_idx = 1;
        std::for_each(
                param_values,
                param_values + n_params,
                [&](const char* value)
                {
                    if (!result.empty())
                    {
                        result.append(", ");
                    }
                    result.append("$" + std::to_string(param_idx) + ":");
                    result.append(value_to_string(value));
                });
    }
    else if (n_params == 0)
    {
        result = "no params";
    }
    else if (n_params == 1)
    {
        result = "one param";
    }
    else
    {
        result = std::to_string(n_params).append(" params");
    }
    return result;
}

void notice_handler(void*, const char* message)
{
    const auto message_length = std::strlen(message);
    if ((0 < message_length) && (message[message_length - 1] == '\n'))
    {
        LIBLOG_INFO("{}", std::string{message, message_length - 1});
    }
    else
    {
        LIBLOG_INFO("{}", message);
    }
}

std::string describe_rows_affected(::PGresult* result)
{
    const char* const rows_affected_str = const_cast<const char*>(::PQcmdTuples(result));
    const bool is_rows_affected_cmd = rows_affected_str[0] != '\0';
    if (!is_rows_affected_cmd)
    {
        return "";
    }
    try
    {
        const auto rows_affected = std::stoull(rows_affected_str);
        switch (rows_affected)
        {
            case 0:
                return "no row affected";
            case 1:
                return " 1 row affected";
        }
        return std::string{rows_affected_str} + " rows affected";
    }
    catch (...)
    {
        return "??? rows affected";
    }
}

std::string describe_cmd(::PGresult* result)
{
    const char* const status_str = const_cast<const char*>(::PQcmdStatus(result));
    const auto rows_affected = describe_rows_affected(result);
    if (rows_affected.empty())
    {
        return status_str;
    }
    return std::string{status_str} + " " + describe_rows_affected(result);
}

std::string describe_tuples(::PGresult* result)
{
    const auto tuples = ::PQntuples(result);
    switch (tuples)
    {
    case 0:
        return "returns no row";
    case 1:
        return "returns 1 row";
    }
    return "returns " + std::to_string(tuples) + " rows";
}

template <typename Duration>
std::string describe_result(::PGresult* result, const Duration& duration)
{
    const std::string duration_str = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    if (result == nullptr)
    {
        return "no result " + duration_str;
    }
    switch (::PQresultStatus(result))
    {
        case PGRES_EMPTY_QUERY:
            return "empty query done in " + duration_str + "ms";
        case PGRES_COMMAND_OK:
            return describe_cmd(result) + " done in " + duration_str + "ms";
        case PGRES_TUPLES_OK:
            return describe_tuples(result) + " in " + duration_str + "ms";
        default:
            return "bad result done in " + duration_str + "ms";
    }
}

class LibPqLogLayer : public LibPg::Detail::LibPqLayerSimpleImplementation
{
public:
    ~LibPqLogLayer() override { }
private:
    class Timer
    {
    public:
        Timer()
            : t0_{std::chrono::steady_clock::now()}
        { }
        decltype(auto) duration() const
        {
            return std::chrono::steady_clock::now() - t0_;
        }
    private:
        const std::chrono::steady_clock::time_point t0_;
    };

    using Base = LibPg::Detail::LibPqLayerSimpleImplementation;
    ::PGconn* PQconnectdbParams(
            const char* const* keywords,
            const char* const* values,
            int expand_dbname) const override
    {
        thread_local int count = 0;
        if (expand_dbname == 0)
        {
            LIBLOG_DEBUG("[{}] \"{}\"", count, serialize_connectdb_params(keywords, values));
        }
        else
        {
            LIBLOG_DEBUG("[{}] \"{}\", expand_dbname = {}", count, serialize_connectdb_params(keywords, values), expand_dbname);
        }
        const Timer timer;
        auto* const result = this->Base::PQconnectdbParams(keywords, values, expand_dbname);
        LIBLOG_DEBUG("[{}] result = ({}) in {}ms", count, static_cast<void*>(result), std::chrono::duration_cast<std::chrono::milliseconds>(timer.duration()).count());
        this->PQsetNoticeProcessor(result, notice_handler, nullptr);
        ++count;
        return result;
    }

    ::PGresult* PQexec(::PGconn* conn, const char* query) const override
    {
        thread_local int count = 0;
        LIBLOG_DEBUG("[{}] \"{}\"", count, query);
        const Timer timer;
        auto* const result = this->Base::PQexec(conn, query);
        LIBLOG_DEBUG("[{}] {}", count, describe_result(result, timer.duration()));
        ++count;
        return result;
    }

    ::PGresult* PQexecParams(
            ::PGconn* conn,
            const char* command,
            int nParams,
            const ::Oid* paramTypes,
            const char* const* paramValues,
            const int* paramLengths,
            const int* paramFormats,
            int resultFormat) const override
    {
        thread_local int count = 0;
        if (resultFormat == 0)
        {
            LIBLOG_DEBUG("[{}] \"{}\", [{}]", count, command, serialize_exec_params(nParams, paramTypes, paramValues, paramLengths, paramFormats));
        }
        else
        {
            LIBLOG_DEBUG("[{}] \"{}\", [{}], resultFormat = {}", count, command, serialize_exec_params(nParams, paramTypes, paramValues, paramLengths, paramFormats), resultFormat);
        }
        const Timer timer;
        auto* const result = this->Base::PQexecParams(conn, command, nParams, paramTypes, paramValues, paramLengths, paramFormats, resultFormat);
        LIBLOG_DEBUG("[{}] {}", count, describe_result(result, timer.duration()));
        ++count;
        return result;
    }

    void PQfinish(::PGconn* conn) const override
    {
        LIBLOG_DEBUG("({})", static_cast<void*>(conn));
        this->Base::PQfinish(conn);
    }
};

LibPg::PgConnection get_connection()
{
    static const class LogLayerInstaller
    {
    public:
        LogLayerInstaller()
            : log_layer_{new LibPqLogLayer{}}
        {
            LibPg::set_libpq_layer(log_layer_);
        }
    private:
        LibPqLogLayer* log_layer_;
    } log_layer_installer{};
    return LibPg::PgConnection{get_dsn()};
}

auto get_registrar(const LibFred::OperationContext& ctx)
{
    return LibFred::Registrar::GetRegistrarHandles{}.exec(ctx).front();
}

}//namespace {anonymous}

BOOST_AUTO_TEST_CASE(use_operation_context_by_libpg)
{
    FREDLOG_SET_CONTEXT(LogCtx, log_ctx, boost::unit_test::framework::current_test_case().full_name());
    auto ro_tx = LibPg::PgRoTransaction{get_connection()};
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
