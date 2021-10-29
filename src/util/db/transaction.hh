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
 *  @file transaction.hh
 *  Interface definition of local automanaged transaction object
 */

#ifndef TRANSACTION_HH_5C53568EE7CD44B188FDBECB85CFAC71
#define TRANSACTION_HH_5C53568EE7CD44B188FDBECB85CFAC71

#include "config.h"

#ifdef HAVE_LOGGER
#include "util/log/log.hh"
#endif

#include "util/db/db_exceptions.hh"
#include "util/db/result.hh"
#include "util/db/connection.hh"

#include <string>
#include <vector>

namespace Database {

/**
 * \class Transaction_
 * \brief Base template class representing local transaction
 *
 * This implementation uses SAVEPOINTS for dealing with nested transactions
 * - in Connection_ we store pointer to base (the most top)
 * transaction created
 */
template <class transaction_type, class manager_type>
class Transaction_
{
public:
    typedef typename manager_type::connection_type connection_type;
    typedef typename manager_type::result_type result_type;
    typedef std::vector<std::string> savepoint_list;

    Transaction_(connection_type& _conn)
        : conn_(_conn),
          ptransaction_(nullptr),
          exited_(false)
    {
        if (!conn_.is_in_transaction())
        {
#ifdef HAVE_LOGGER
            FREDLOG_DEBUG(boost::format("(%1%) start transaction request -- begin") % this);
#endif
            this->exec(transaction_.start());
            conn_.setTransaction(this);
        }
        else
        {
#ifdef HAVE_LOGGER
            FREDLOG_DEBUG(boost::format("(%1%) start transaction request -- (%2%) already active") % this % conn_.getTransaction());
#endif
            this->set_parent_transaction(conn_.getTransaction());
            conn_.setTransaction(this);
            this->savepoint();
        }
    }

    ~Transaction_()
    {
        this->rollback();
    }

    void rollback()noexcept
    {
        if (!exited_)
        {
            try
            {
                if (ptransaction_ == nullptr)
                {
#ifdef HAVE_LOGGER
                    FREDLOG_DEBUG(boost::format("(%1%) rollback transaction request -- rollback") % this);
#endif
                    this->exec(transaction_.rollback());
                    conn_.unsetTransaction();
                }
                else
                {
#ifdef HAVE_LOGGER
                    FREDLOG_DEBUG(boost::format("(%1%) rollback transaction request -- to savepoint") % this);
#endif
                    conn_.setTransaction(ptransaction_);
                    this->exec(transaction_.rollback() + " TO SAVEPOINT " + savepoints_.front());
                }
            }
            catch (const Database::Exception &e)
            {
#ifdef HAVE_LOGGER
                FREDLOG_DEBUG(boost::format("(%1%) Rollback failed: %2% ") % this % e.what());
#endif
            }
            catch (...)
            {
#ifdef HAVE_LOGGER
                FREDLOG_DEBUG(boost::format("(%1%) rollback failed - unknown excepiton") % this);
#endif
            }
            exited_ = true;
        }
    }

    void commit()
    {
        if (!exited_)
        {
            if (ptransaction_ != nullptr)
            {
#ifdef HAVE_LOGGER
                FREDLOG_DEBUG(boost::format("(%1%) commit transaction request -- release savepoint") % this);
#endif
                conn_.exec("RELEASE SAVEPOINT " + savepoints_.front());
                conn_.setTransaction(ptransaction_);
            }
            else if (conn_.getTransaction() == this)
            {
#ifdef HAVE_LOGGER
                FREDLOG_DEBUG(boost::format("(%1%) commit transaction request -- commit ok") % this);
#endif
                this->exec(transaction_.commit());
                conn_.unsetTransaction();
            }
            else
            {
#ifdef HAVE_LOGGER
                FREDLOG_ERROR(boost::format("(%1%) commit transaction request -- child active!") % this);
#endif
            }
            exited_ = true;
        }
    }

    void prepare(const std::string& _id)
    {
        if (ptransaction_ != nullptr)
        {
            throw std::runtime_error("cannot call prepare transaction on nested transaction");
        }
        if (_id.empty())
        {
            throw std::runtime_error("cannot call prepare transaction without id");
        }

        if (!exited_)
        {
            this->exec(transaction_.prepare(_id));
            conn_.unsetTransaction();
            exited_ = true;
        }
    }

    result_type exec(const std::string& _query)
    {
        return conn_.exec(_query);
    }

    void savepoint(std::string _name = std::string())
    {
        if (_name.empty())
        {
            _name = this->generate_savepoint_name();
        }
        savepoints_.push_back(_name);
        conn_.exec("SAVEPOINT " + _name);
    }
private:
    connection_type& conn_;
    Transaction_* ptransaction_;
    transaction_type transaction_;
    bool exited_;
    savepoint_list savepoints_;

    savepoint_list::size_type get_next_savepoint_num()const
    {
        return savepoints_.size();
    }

    std::string generate_savepoint_name()const
    {
        auto num = savepoints_.size();
        if (ptransaction_ != nullptr)
        {
            num = std::max(num, ptransaction_->get_next_savepoint_num());
        }

        return str(boost::format("sp%1%") % num);
    }

    void set_parent_transaction(Transaction_* _trans)
    {
        ptransaction_ = _trans;
#ifdef HAVE_LOGGER
        FREDLOG_DEBUG(boost::format("(%1%) parent transaction assigned (%2%)") % this % ptransaction_);
#endif
    }
};

}//namespace Database

#endif//TRANSACTION_HH_5C53568EE7CD44B188FDBECB85CFAC71
