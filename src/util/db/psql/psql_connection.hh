/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
 *  @file psql_connection.hh
 *  Implementation of connection object for PSQL database.
 */

#ifndef PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC
#define PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC

#include "util/db/psql/psql_result.hh"
#include "util/db/query_param.hh"

#include <libpq-fe.h>

#include <istream>
#include <string>

namespace Database {

class TerribleHack;

/**
 * \class PSQLConnection
 * \brief PSQL connection driver for Connection_ template
 */
class PSQLConnection
{
public:
    using ResultType = PSQLResult;
    using OpenType = std::string;

    PSQLConnection() = delete;
    PSQLConnection(const PSQLConnection&) = delete;
    PSQLConnection& operator=(const PSQLConnection&) = delete;

    explicit PSQLConnection(const OpenType& need_to_open);// throw (ConnectionFailed)

    virtual ~PSQLConnection();

    static std::string to_publicable_string(const OpenType& need_to_open);

    void close()noexcept;

    ResultType exec(const std::string& query);// throw (ResultFailed)

    ResultType exec_params(
            const std::string& query, //one command query
            const std::vector<std::string>& params);//parameters data

    ResultType exec_params(
            const std::string& _query, //one command query
            const QueryParams& params);//parameters data

    ResultType copy_from(std::istream& input_data, const std::string& table_name, std::size_t buffer_size);

    void setQueryTimeout(unsigned t);

    void reset();

    std::string escape(const std::string& in)const;

    bool is_in_transaction()const;

    bool is_in_valid_transaction()const;
private:
    explicit PSQLConnection(PGconn* conn);// used by the TerribleHack class
    PGconn* psql_conn_; ///< wrapped connection structure from libpq library
    friend class TerribleHack;
};

}//namespace Database

#endif//PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC
