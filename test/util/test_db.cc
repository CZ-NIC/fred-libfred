/*
 * Copyright (C) 2020  CZ.NIC, z. s. p. o.
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
#include "src/libfred/opcontext.hh"

#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <array>
#include <string>
#include <sstream>

namespace {

template <typename T, std::size_t R, std::size_t C>
using Table = std::array<std::array<T, C>, R>;


struct PgCopyParams
{
    PgCopyParams(const char column_delimiter, std::string null_value)
        : column_delimiter(column_delimiter),
          null_value(std::move(null_value)) {}

    char column_delimiter;
    std::string null_value;
};


static const auto pg_copy_textmode_defaults = PgCopyParams('\t', "\\N");


std::string to_string(const std::string& in)
{
    return in;
}


std::string make_copy_input(const auto& table, const PgCopyParams& params)
{
    std::string aux;
    for (const auto& row : table)
    {
        std::string tmp_row;
        for (const auto& value : row)
        {
            if (!tmp_row.empty())
            {
                tmp_row += params.column_delimiter;
            }
            tmp_row += to_string(value);
        }
        if (!aux.empty())
        {
            aux += '\n';
        }
        aux += tmp_row;
    }
    return aux;
}


void check_result(const Database::Result& result, const auto& table, const PgCopyParams& params)
{
    for (auto r_idx = 0u; r_idx < result.size(); ++r_idx)
    {
        for (auto c_idx = 0u; c_idx < result[r_idx].size(); ++c_idx)
        {
            const auto value = result[r_idx][c_idx];
            if (value.isnull())
            {
                BOOST_CHECK_EQUAL(params.null_value, table[r_idx][c_idx]);
            }
            else
            {
                BOOST_CHECK_EQUAL(static_cast<std::string>(value), table[r_idx][c_idx]);
            }
        }
    }
}


void print_result(const Database::Result& result)
{
    for (auto r_idx = 0u; r_idx < result.size(); ++r_idx)
    {
        for (auto c_idx = 0u; c_idx < result[r_idx].size(); ++c_idx)
        {
            const auto value = result[r_idx][c_idx];
            std::cout << (value.isnull() ? "[null]" : static_cast<std::string>(value)) << "\t";
        }
        std::cout << "\n";
    }

}


std::string get_non_existent_tablename(LibFred::OperationContext& ctx, const std::string& prefix)
{
    std::string nonexistent_tablename;
    int counter = 0;
    bool table_exists = true;
    do
    {
        nonexistent_tablename = prefix + std::to_string(counter);
        const auto result = ctx.get_conn().exec_params(
                "SELECT True FROM pg_tables WHERE LOWER(tablename) = LOWER($1::TEXT) AND schemaname = 'public'",
                Database::query_param_list(nonexistent_tablename));
        table_exists = result.size() > 0;
        ++counter;
    } while (table_exists);
    return nonexistent_tablename;
}

}


BOOST_AUTO_TEST_SUITE(Tests)
BOOST_AUTO_TEST_SUITE(Util)
BOOST_AUTO_TEST_SUITE(Db)

BOOST_FIXTURE_TEST_CASE(test_copy_from_ok_empty_input, Test::instantiate_db_template)
{
    std::istringstream copy_input(std::string{});

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    ctx.get_conn().copy_from(copy_input, table_name);
    const auto result = ctx.get_conn().exec("SELECT value FROM " + table_name);

    BOOST_CHECK(result.size() == 0);
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_ok, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 1> input = {{{"foo"}, {"bar"}, {"baz"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    ctx.get_conn().copy_from(copy_input, table_name);
    const auto result = ctx.get_conn().exec("SELECT value FROM " + table_name);

    check_result(result, input, copy_params);
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_ok_multi_column, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 3> input = {{{"foo1", "foo2", "foo3"}, {"bar1", "bar2", "bar3"}, {"baz1", "baz2", "baz3"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value1 TEXT, value2 TEXT, value3 TEXT)");
    ctx.get_conn().copy_from(copy_input, table_name);
    const auto result = ctx.get_conn().exec("SELECT value1, value2, value3 FROM " + table_name);

    check_result(result, input, copy_params);
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_ok_null_values, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 1> input = {{{"foo"}, {copy_params.null_value}, {"baz"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    ctx.get_conn().copy_from(copy_input, table_name);
    const auto result = ctx.get_conn().exec("SELECT value FROM " + table_name);

    check_result(result, input, copy_params);
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_fail_bad_input, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 1> input = {{{std::string{"foo"} + copy_params.column_delimiter + "moo"}, {copy_params.null_value}, {"baz"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().copy_from(copy_input, table_name),
            Database::ResultFailed,
            [](const auto& ex) {
                BOOST_TEST_MESSAGE(ex.what());
                return std::string{ex.what()}.size() > 0;
            });
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_fail_no_table, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 1> input = {{{"foo"}, {"bar"}, {"baz"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().copy_from(copy_input, get_non_existent_tablename(ctx, "non_existent_tablename")),
            Database::ResultFailed,
            [](const auto& ex) {
                BOOST_TEST_MESSAGE(ex.what());
                return std::string{ex.what()}.size() > 0;
            });
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_fail_tx_usage, Test::instantiate_db_template)
{
    const auto copy_params = pg_copy_textmode_defaults;
    const Table<std::string, 3, 1> input = {{{std::string{"foo"} + copy_params.column_delimiter + "moo"}, {copy_params.null_value}, {"baz"}}};
    std::istringstream copy_input(make_copy_input(input, copy_params));

    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    const auto pre_savepoint_value = "muhehehe";
    ctx.get_conn().exec_params("INSERT INTO " + table_name + " VALUES ($1::TEXT)", Database::query_param_list(pre_savepoint_value));
    ctx.get_conn().exec("SAVEPOINT s");
    // COPY with invalid data should break transaction
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().copy_from(copy_input, table_name),
            Database::ResultFailed,
            [](const auto& ex) { return std::string{ex.what()}.size() > 0; });
    // check that transaction is unusable
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().exec("SELECT value FROM " + table_name),
            Database::ResultFailed,
            [](const auto& ex) {
                BOOST_TEST_MESSAGE(ex.what());
                return std::string{ex.what()}.size() > 0;
            });
    // should be able to recover using savepoint
    ctx.get_conn().exec("ROLLBACK TO SAVEPOINT s");
    // check transaction is usable with pre savepoint data retrieval
    const auto result = ctx.get_conn().exec("SELECT value FROM " + table_name);
    BOOST_CHECK(result.size() == 1);
    BOOST_CHECK_EQUAL(static_cast<std::string>(result[0][0]), pre_savepoint_value);
};


BOOST_FIXTURE_TEST_CASE(test_copy_from_fail_tx_broken_stream, Test::instantiate_db_template)
{
    std::istringstream broken_copy_input{"foo"};
    broken_copy_input.setstate(std::ios::badbit);
    LibFred::OperationContextCreator ctx;
    const auto table_name = get_non_existent_tablename(ctx, "foo");
    ctx.get_conn().exec("CREATE TEMPORARY TABLE " + table_name + " (value TEXT)");
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().copy_from(broken_copy_input, table_name),
            Database::ResultFailed,
            [](const auto& ex) {
                BOOST_TEST_MESSAGE(ex.what());
                return std::string{ex.what()} == std::string{"Result failed: COPY FROM failed - Input data read error"};
            });
    // check that transaction is unusable
    BOOST_CHECK_EXCEPTION(
            ctx.get_conn().exec("SELECT value FROM " + table_name),
            Database::ResultFailed,
            [](const auto& ex) {
                BOOST_TEST_MESSAGE(ex.what());
                return std::string{ex.what()}.size() > 0;
            });
};

BOOST_AUTO_TEST_SUITE_END()//Tests/Util/Db
BOOST_AUTO_TEST_SUITE_END()//Tests/Util
BOOST_AUTO_TEST_SUITE_END()//Tests
