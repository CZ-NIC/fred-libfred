/*
 * Copyright (C) 2007  CZ.NIC, z.s.p.o.
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
 *  @file manager_tss.h
 *  \brief Storing Connection as a thread specific data
 */

#ifndef MANAGER_TSS_HH_BF0788B58F294031ACB5E438E48E058E
#define MANAGER_TSS_HH_BF0788B58F294031ACB5E438E48E058E

#include "src/util/db/result.hh"
#include "src/util/db/connection.hh"
#include "src/util/db/transaction.hh"

#include <boost/thread/thread.hpp>
#include <boost/thread/tss.hpp>

#include <string>
#include <stdexcept>

namespace Database {

/**
 * \class TSSManager_
 * \brief Database connection manager for storing and retrieving connections
 *        from thread local data
 *
 * @param connection_factory  connection factory class
 */
template <class connection_factory>
class TSSManager_
{
public:
  typedef typename connection_factory::connection_driver                          connection_driver;
  typedef TSSConnection_<connection_driver, TSSManager_>                          connection_type;
  typedef Transaction_<typename connection_driver::transaction_type, TSSManager_> transaction_type;
  typedef Result_<typename connection_driver::result_type>                        result_type;
  typedef Sequence_<connection_type, TSSManager_>                                 sequence_type;
  typedef typename result_type::Row                                               row_type;

  /**
   * Connection factory object initialization
   *
   * @param _conn_factory  factory to be assigned to manager; it has to be already initialized
   */
  static void init(connection_factory *_conn_factory)
  {
    conn_factory_ = _conn_factory;
    init_ = true;
  }

  /**
   * Acquire database connection for actual thread
   *
   * @return  database connection handler pointer
   */
  static connection_type acquire()
  {
    PerThreadData_ *tmp = data_.get();
    if (tmp != nullptr)
    {
      update_data_(tmp);
#ifdef HAVE_LOGGER
      LOGGER(PACKAGE).debug(boost::format("[tss] acquire state: conn=%1%  trans=%2%") % tmp->conn % &tmp->trans);
#endif
      return connection_type(tmp->conn, tmp->trans);
    }
    data_.reset(new PerThreadData_());
    return acquire();
  }

  static const std::string& getConnectionString()
  {
    if (conn_factory_ == nullptr)
    {
        throw std::runtime_error("Uninitialized connection factory");
    }
    return conn_factory_->getConnectionString();
  }

  /**
   * Explicit release database connection for actual thread
   * back to pool
   *
   * TODO: release policy - 1. do nothing (connection stay in thread until it exits)
   *                        2. factory release (i.e. to pool)
   */
  static void release()
  {
    PerThreadData_* const tmp = data_.get();
    if (tmp != nullptr)
    {
#ifdef HAVE_LOGGER
      LOGGER(PACKAGE).debug(boost::format("[tss] release state: conn=%1%  trans=%2%") % tmp->conn % &tmp->trans);
#endif
      if (tmp->conn != nullptr)
      {
         conn_factory_->release(tmp->conn);
         tmp->conn = nullptr;
      }
    }
  }
private:
  TSSManager_() { }

  ~TSSManager_() { }

  /**
   * Stucture to store per thread connection and transaction info
   */
  struct PerThreadData_
  {
    PerThreadData_() : conn(0), trans(0) { }

    ~PerThreadData_()
    {
      if ((conn != nullptr) && (conn_factory_ != nullptr))
      {
        conn_factory_->release(conn);
      }
    }

    connection_driver *conn;
    transaction_type  *trans;
  };

  /**
   * Helper method for acquiring database connection
   */
  static void update_data_(PerThreadData_*& _data)
  {
    if (_data->conn == nullptr)
    {
      _data->conn = conn_factory_->acquire();
      _data->trans = 0;
    }
  }

  static connection_factory*                        conn_factory_; /**< connection factory */
  static boost::thread_specific_ptr<PerThreadData_> data_;         /**< data per thread structure */
  static bool                                       init_;         /**< data ready initialized internal flag */
};


/**
 * static members initialization
 */
template <class connection_factory>
connection_factory* TSSManager_<connection_factory>::conn_factory_ = nullptr;

template <class connection_factory>
boost::thread_specific_ptr<typename TSSManager_<connection_factory>::PerThreadData_> TSSManager_<connection_factory>::data_;

template <class connection_factory>
bool TSSManager_<connection_factory>::init_ = false;

}//namespace Database

#endif /*DATABASE_MANAGER_TSS_H_*/
