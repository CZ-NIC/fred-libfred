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
 *  @file
 *  operation context
 */

#ifndef OPCONTEXT_HH_313D7DA5889A4FCAA339EF8F775103C4
#define OPCONTEXT_HH_313D7DA5889A4FCAA339EF8F775103C4

#include "libfred/db_settings.hh"

#include <string>

//Forward declarations of LibPg transaction classes
namespace LibPg {

class PgTransaction;
class PgRoTransaction;
class PgRwTransaction;

}//namespace LibPg

namespace LibFred {

class OperationContextCreator;
class OperationContextTwoPhaseCommit;
class OperationContextTwoPhaseCommitCreator;

/**
 * Common objects needed in Fred operations. It consists of one part, database.
 *
 * Is non-copyable.
 * Is not directly instantiable (implemented by private ctor and destructor).
 * Is instantiable only by selected friend classes.
 */
class OperationContext
{
public:
    // Implicitly constructible from LibPg transaction classes
    OperationContext(const LibPg::PgTransaction& tx);
    OperationContext(const LibPg::PgRoTransaction& ro_tx);
    OperationContext(const LibPg::PgRwTransaction& rw_tx);

    OperationContext(OperationContext&& src);

    OperationContext(const OperationContext&) = delete;

    ~OperationContext();

    OperationContext& operator=(const OperationContext&) = delete;
    OperationContext& operator=(OperationContext&&) = delete;

    using DbConn = Database::StandaloneConnection;
    /**
     * Obtain database connection with running transaction.
     * @return database connection object reference
     * @throw std::runtime_error if no transaction in progress
     */
    DbConn& get_conn()const;
private:
    OperationContext();
    OperationContext(std::unique_ptr<DbConn> conn);
    std::unique_ptr<DbConn> conn_;
    friend class OperationContextCreator;
    friend class OperationContextTwoPhaseCommit;
    friend class OperationContextTwoPhaseCommitCreator;
};

/**
 * OperationContext with two-phase commit only.
 *
 * Is non-copyable (implemented by non-copyable base class).
 * Is not directly instantiable (implemented by private ctor and destructor).
 * Is instantiable only by friend class.
 */
class OperationContextTwoPhaseCommit : public OperationContext
{
public:
    /**
     * Database transaction string identification which will be used in first phase of two-phase commit.
     */
    std::string get_transaction_id()const { return transaction_id_; }
private:
    OperationContextTwoPhaseCommit(const std::string &_transaction_id);
    ~OperationContextTwoPhaseCommit() { }
    const std::string transaction_id_;
    friend class OperationContextTwoPhaseCommitCreator;
};

enum class DbLock
{
    for_share,
    for_update
};

/**
 * Joins database locking type with operation context.
 */
template <DbLock>
class OperationContextUsing
{
public:
    explicit OperationContextUsing(OperationContext& ctx) : ctx_(ctx) { }
    operator OperationContext&()const { return ctx_; }
private:
    OperationContext& ctx_;
};

using OperationContextLockingForShare = OperationContextUsing<DbLock::for_share>;
using OperationContextLockingForUpdate = OperationContextUsing<DbLock::for_update>;

/**
 * Creates OperationContext instance and offers commit_transaction() method.
 *
 * Is non-copyable (implemented by non-copyable base class).
 */
class OperationContextCreator : public OperationContext
{
public:
    /**
     * Creates logging object and starts database transaction without two-phase commit ability.
     */
    OperationContextCreator() { }
    /**
     * Processes database transaction rollback if transaction wasn't commited.
     */
    ~OperationContextCreator() { }
    /**
     * Commits database transaction.
     * @throw std::runtime_error if no transaction in progress
     */
    void commit_transaction();
};

/**
 * Creates OperationContextTwoPhaseCommit instance and offers commit_transaction() method.
 *
 * Is non-copyable (implemented by non-copyable base class).
 */
class OperationContextTwoPhaseCommitCreator : public OperationContextTwoPhaseCommit
{
public:
    /**
     * Creates logging object and starts database transaction prepared for two-phase commit.
     * @param _transaction_id database transaction string identification usable in second phase commit/rollback
     * @throw std::runtime_error in case of empty _transaction_id
     * @note Calling commit_transaction() will process first phase of two-phase commit.
     */
    OperationContextTwoPhaseCommitCreator(const std::string& _transaction_id)
        : OperationContextTwoPhaseCommit(_transaction_id) { }
    /**
     * Processes database transaction rollback if transaction wasn't commited.
     */
    ~OperationContextTwoPhaseCommitCreator() { }
    /**
     * Processes first phase of two-phase commit.
     * @throw std::runtime_error if no transaction in progress
     */
    void commit_transaction();
};

/**
 * Processes second phase of two-phase database transaction commit on standalone database connection.
 * @param _transaction_id database transaction string identification used in first phase of two-phase commit
 * @throw std::runtime_error if _transaction_id empty
 */
void commit_transaction(const std::string& _transaction_id);

/**
 * Drops two-phase database transaction on standalone database connection.
 * @param _transaction_id database transaction string identification used in first phase of two-phase commit
 * @throw std::runtime_error if _transaction_id empty
 */
void rollback_transaction(const std::string& _transaction_id);

} // namespace LibFred

#endif
