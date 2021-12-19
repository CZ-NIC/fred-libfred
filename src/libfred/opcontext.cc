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
 *  @file opcontext.cc
 *  operation context
 */

#include "libfred/opcontext.hh"

#include "src/util/log/log.hh"

#include "libpg/pg_ro_transaction.hh"
#include "libpg/pg_rw_transaction.hh"
#include "libpg/pg_transaction.hh"

#include <stdexcept>
#include <utility>

namespace Database {

class TerribleHack : public PSQLConnection
{
public:
    template <typename Tx>
    explicit TerribleHack(const Tx& tx)
        : PSQLConnection{pg_conn_from_transaction(tx)} // do not transfer the PGconn* ownership
    {}
    ~TerribleHack() override
    {
        psql_conn_ = nullptr;// prevent PGconn* deletion
    }
private:
    template <typename Tx>
    static PGconn* pg_conn_from_transaction(const Tx& tx)
    {
        return reinterpret_cast<PGconn*>(LibPg::Unsafe::Dirty::Hack::get_raw_pointer(tx));
    }
};

using PSQLConnectionWithoutOwnership = TerribleHack;

}//namespace Database

namespace LibFred {

namespace {

const std::string& check_transaction_id(const std::string &_value)
{
    if (_value.empty())
    {
        throw std::runtime_error("prepared transaction id mustn't be empty");
    }
    constexpr auto max_length_of_transaction_id = 200u;
    if (max_length_of_transaction_id < _value.length())
    {
        throw std::runtime_error("prepared transaction id too long");
    }
    //Postgres PREPARE TRANSACTION commands family doesn't accept parameters, requires immediate argument,
    //so I check unsafe character(s) in transaction identifier.
    if (_value.find('\'') != std::string::npos)
    {
        throw std::runtime_error("prepared transaction id too unsafe");
    }
    return _value;
}

std::unique_ptr<Database::StandaloneConnection> get_database_conn()
{
    return Database::get_default_manager<Database::StandaloneConnectionFactory>().acquire();
}

template <typename Tx>
auto make_psql_connection_without_ownership(const Tx& tx)
{
    return static_cast<std::unique_ptr<Database::PSQLConnection>>(
            std::make_unique<Database::PSQLConnectionWithoutOwnership>(tx));
}

template <typename Tx>
auto make_standalone_connection_without_ownership(const Tx& tx)
{
    return std::make_unique<Database::StandaloneConnection>(
            make_psql_connection_without_ownership(tx).release());
}

}//namespace LibFred::{anonymous}

OperationContext::OperationContext()
    : conn_(get_database_conn())
{
    conn_->exec("START TRANSACTION ISOLATION LEVEL READ COMMITTED");
}

OperationContext::OperationContext(OperationContext&& src)
    : conn_{std::move(src.conn_)}
{}

OperationContext::OperationContext(const LibPg::PgTransaction& tx)
    : conn_{make_standalone_connection_without_ownership(tx)}
{}

OperationContext::OperationContext(const LibPg::PgRoTransaction& ro_tx)
    : conn_{make_standalone_connection_without_ownership(ro_tx)}
{}

OperationContext::OperationContext(const LibPg::PgRwTransaction& rw_tx)
    : conn_{make_standalone_connection_without_ownership(rw_tx)}
{}


Database::StandaloneConnection& OperationContext::get_conn()const
{
    Database::StandaloneConnection* const conn_ptr = conn_.get();
    if (conn_ptr != nullptr)
    {
        if (conn_ptr->is_in_transaction())
        {
            return *conn_ptr;
        }
        throw std::runtime_error("database transaction broken");
    }
    throw std::runtime_error("database connection doesn't exist");
}

OperationContext::~OperationContext()
{
    // Database::StandaloneConnection = Connection_<Database::PSQLConnection, ...>
    Database::StandaloneConnection *const conn_ptr = conn_.get();
    if (conn_ptr == nullptr)
    {
        return;
    }
    const bool skip_rollback =
            conn_ptr->is_derived_from<Database::PSQLConnectionWithoutOwnership>();
    if (!skip_rollback)
    {
        try
        {
            conn_ptr->exec("ROLLBACK");
        }
        catch (...)
        {
            try
            {
                FREDLOG_ERROR("rollback failed");
            }
            catch (...) { }
        }
    }
    try
    {
        conn_.reset();
    }
    catch (...)
    {
        try
        {
            FREDLOG_ERROR("database connection destroying failed");
        }
        catch (...) { }
    }
}

OperationContextTwoPhaseCommit::OperationContextTwoPhaseCommit(const std::string& _transaction_id)
    : transaction_id_(check_transaction_id(_transaction_id))
{ }

void OperationContextCreator::commit_transaction()
{
    this->get_conn().exec("COMMIT");
    conn_.reset();
}

void OperationContextTwoPhaseCommitCreator::commit_transaction()
{
    //"PREPARE TRANSACTION $1::TEXT" failed
    this->get_conn().exec("PREPARE TRANSACTION '" + transaction_id_ + "'");
    conn_.reset();
}

void commit_transaction(const std::string& _transaction_id)
{
    check_transaction_id(_transaction_id);
    std::unique_ptr<Database::StandaloneConnection> conn_ptr = get_database_conn();
    //"COMMIT PREPARED $1::TEXT" failed
    conn_ptr->exec("COMMIT PREPARED '" + _transaction_id + "'");
}

void rollback_transaction(const std::string &_transaction_id)
{
    check_transaction_id(_transaction_id);
    std::unique_ptr< Database::StandaloneConnection > conn_ptr = get_database_conn();
    //"ROLLBACK PREPARED $1::TEXT" failed
    conn_ptr->exec("ROLLBACK PREPARED '" + _transaction_id + "'");
}

}//namespace LibFred
