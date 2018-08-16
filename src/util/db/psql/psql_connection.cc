/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file psql_connection.cc
 *  Implementation of connection object for PSQL database.
 */

#include "src/util/db/psql/psql_connection.hh"
#include "src/util/db/db_exceptions.hh"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <algorithm>

namespace Database {

PSQLConnection::PSQLConnection(const OpenType& need_to_open)
    : psql_conn_(PQconnectdb(need_to_open.c_str()))
{
    if (PQstatus(psql_conn_) != CONNECTION_OK)
    {
        const std::string err_msg = PQerrorMessage(psql_conn_);
        this->close();
        throw ConnectionFailed(to_publicable_string(need_to_open) + " errmsg: " + err_msg);
    }
#ifdef HAVE_LOGGER
    struct Logger
    {
        static void notice_processor(void*, const char* message)
        {
            // replace new line symbols
            std::string msg(message);
            std::replace(msg.begin(), msg.end(), '\n', ' ');
            LOGGER(PACKAGE).debug(msg);
        }
    };
    // set notice processor
    PQsetNoticeProcessor(psql_conn_, Logger::notice_processor, NULL);
#endif
}

PSQLConnection::~PSQLConnection()
{
    this->close();
}

std::string PSQLConnection::to_publicable_string(const OpenType& need_to_open)
{
    static const boost::regex re_find_password("password=[^\\ ]+");
    return boost::regex_replace(need_to_open, re_find_password, "password=******** ");
}

void PSQLConnection::close()noexcept
{
    if (psql_conn_ != nullptr)
    {
        PQfinish(psql_conn_);
        psql_conn_ = nullptr;
    }
}

PSQLConnection::ResultType PSQLConnection::exec(const std::string& _query)
{
    const auto tmp = std::shared_ptr<PGresult>(PQexec(psql_conn_, _query.c_str()), PQclear);

    const ExecStatusType status = PQresultStatus(tmp.get());
    if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
    {
        return PSQLResult(tmp);
    }
    throw ResultFailed(_query + " (" + PQerrorMessage(psql_conn_) + ")");
}

PSQLConnection::ResultType PSQLConnection::exec_params(
        const std::string& query,
        const std::vector<std::string>& params)
{
    std::vector<const char*> param_values; //pointer to memory with parameters data
    constexpr const Oid* untyped_literal_strings = nullptr;
    std::vector<int> param_lengths;
    constexpr const int* all_parameters_are_text_strings = nullptr;
    constexpr int results_in_text_format = 0;

    param_values.reserve(params.size());
    param_lengths.reserve(params.size());
    for (const auto& param : params)
    {
        param_values.push_back(param.c_str());
        param_lengths.push_back(param.size());
    }

    const auto tmp = std::shared_ptr<PGresult>(
            PQexecParams(
                    psql_conn_,
                    query.c_str(),
                    param_values.size(),
                    untyped_literal_strings,
                    param_values.data(),
                    param_lengths.data(),
                    all_parameters_are_text_strings,
                    results_in_text_format),
            PQclear);

    const ExecStatusType status = PQresultStatus(tmp.get());
    if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
    {
        return PSQLResult(tmp);
    }
    std::string params_dump;
    std::size_t params_counter = 0;
    for (const auto& param : params)
    {
        ++params_counter;
        params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " + param;
    }

    throw ResultFailed("query: " + query + " "
                       "Params:" + params_dump + " "
                       "(" + PQerrorMessage(psql_conn_) + ")");
}

PSQLConnection::ResultType PSQLConnection::exec_params(
        const std::string& query,
        const QueryParams& params)
{
    constexpr Oid an_untyped_literal_string = 0;
    constexpr Oid a_binary_data = 17;
    constexpr int results_in_text_format = 0;
    constexpr int parameter_is_text = 0;
    constexpr int parameter_is_binary = 1;

    std::vector<Oid> param_types;//types of query parameters
    std::vector<const char*> param_values; //pointer to memory with parameters data
    std::vector<int> param_lengths; //sizes of memory with parameters data
    std::vector<int> param_formats; //format of parameter data

    param_types.reserve(params.size());
    param_values.reserve(params.size());
    param_lengths.reserve(params.size());
    param_formats.reserve(params.size());
    for (const auto& param : params)
    {
        param_types.push_back(param.is_binary() ? a_binary_data : an_untyped_literal_string);
        param_values.push_back(param.is_null() ? nullptr : &(param.get_data())[0]);
        param_lengths.push_back(param.get_data().size());
        param_formats.push_back(param.is_binary() ? parameter_is_binary : parameter_is_text);
    }

    const auto tmp = std::shared_ptr<PGresult>(
            PQexecParams(
                    psql_conn_,
                    query.c_str(),
                    param_values.size(),
                    param_types.size() != 0 ? param_types.data() : nullptr,
                    param_values.data(),
                    param_lengths.data(),
                    param_formats.data(),
                    results_in_text_format),
            PQclear);

    const ExecStatusType status = PQresultStatus(tmp.get());
    if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
    {
        return PSQLResult(tmp);
    }

    std::string params_dump;
    std::size_t params_counter = 0;
    for (const auto& param : params)
    {
        ++params_counter;
        params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " +
                       (param.is_null() ? std::string("null")
                                        : (param.is_binary() ? std::string("binary") : param.get_data()));
    }

    throw ResultFailed("query: " + query + " "
                       "Params:" + params_dump + " (" + PQerrorMessage(psql_conn_) + ")");
}

void PSQLConnection::setQueryTimeout(unsigned t)
{
    boost::format fmt_timeout = boost::format("SET statement_timeout=%1%") % t;
    this->exec(fmt_timeout.str());
}

void PSQLConnection::reset()
{
    if (psql_conn_ != nullptr)
    {
        PQreset(psql_conn_);
    }
}

std::string PSQLConnection::escape(const std::string& from)const
{
    int error_code;
    const std::unique_ptr<char[]> to(new char[(2 * from.size()) + 1]);
    const ::size_t bytes = PQescapeStringConn(psql_conn_, to.get(), from.c_str(), from.size(), &error_code);
    const bool success = error_code == 0;
    if (success)
    {
        return std::string(to.get(), bytes);
    }
    const std::string msg = boost::str(boost::format("error in escape function: %1%") % PQerrorMessage(psql_conn_));
#ifdef HAVE_LOGGER
    LOGGER(PACKAGE).error(msg);
#endif
    throw std::runtime_error(msg);
}

bool PSQLConnection::is_in_transaction()const
{
    switch (PQtransactionStatus(psql_conn_))
    {
        case PQTRANS_INTRANS://idle, within transaction block
        case PQTRANS_INERROR://idle, within failed transaction
            return true;
        case PQTRANS_IDLE:   //connection idle
        case PQTRANS_ACTIVE: //command in progress
        case PQTRANS_UNKNOWN://cannot determine status
            return false;
    }
    throw std::runtime_error("PQtransactionStatus() failure: unexpected return value");
}

bool PSQLConnection::is_in_valid_transaction()const
{
    switch (PQtransactionStatus(psql_conn_))
    {
        case PQTRANS_INTRANS://idle, within transaction block
            return true;
        case PQTRANS_INERROR://idle, within failed transaction
        case PQTRANS_IDLE:   //connection idle
        case PQTRANS_ACTIVE: //command in progress
        case PQTRANS_UNKNOWN://cannot determine status
            return false;
    }
    throw std::runtime_error("PQtransactionStatus() failure: unexpected return value");
}

}//namespace Database
