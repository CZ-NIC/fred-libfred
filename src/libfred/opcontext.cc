/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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

#include <stdexcept>

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

}//namespace LibFred::{anonymous}

OperationContext::OperationContext()
    : conn_(get_database_conn()),
      log_(LOGGER)
{
    conn_->exec("START TRANSACTION ISOLATION LEVEL READ COMMITTED");
}

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
    Database::StandaloneConnection *const conn_ptr = conn_.get();
    if (conn_ptr == nullptr)
    {
        return;
    }
    try
    {
        conn_ptr->exec("ROLLBACK");
    }
    catch (...)
    {
        try
        {
            log_.error("OperationContext::~OperationContext: rollback failed");
        }
        catch (...) { }
    }
    try
    {
        conn_.reset();
    }
    catch (...)
    {
        try
        {
            log_.error("OperationContext::~OperationContext: database connection destroying failed");
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
