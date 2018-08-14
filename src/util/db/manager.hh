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
 *  @file manager.hh
 *  Manager_ for database connections.
 */

#ifndef MANAGER_HH_7B50C0E33D764413851378E6AB696A4E
#define MANAGER_HH_7B50C0E33D764413851378E6AB696A4E

#include "config.h"

#include "src/util/db/result.hh"
#include "src/util/db/connection.hh"
#include "src/util/db/transaction.hh"
#include "src/util/db/sequence.hh"

#ifdef HAVE_LOGGER
#include "src/util/log/logger.hh"
#endif

#include <memory>
#include <stdexcept>
#include <string>

namespace Database {

/**
 * \class Manager_
 * \brief Simple database manager
 *
 * This object populated with specific connection factory can be used as
 * connection manager
 */
template <class T>
class Manager_
{
public:
    using connection_driver = typename T::connection_driver;
    using connection_type = Connection_<connection_driver, Manager_>;
    using transaction_type = Transaction_<typename connection_driver::transaction_type, Manager_>;
    using result_type = Result_<typename connection_driver::result_type>;
    using sequence_type = Sequence_<connection_type, Manager_>;
    using row_type = typename result_type::Row;

    static std::unique_ptr<Manager_> make_instance()
    {
        std::string& connection_string = get_connection_string();
        if (connection_string.empty())
        {
            throw std::runtime_error("Connection string not yet initialized");
        }
        std::unique_ptr<T> conn_factory(std::make_unique<T>(connection_string));
        std::unique_ptr<Manager_> manager(new Manager_(std::move(conn_factory)));
        return std::move(manager);
    }
    ~Manager_() = default;

    const std::string& getConnectionString()
    {
        return conn_factory_->getConnectionString();
    }

    std::unique_ptr<connection_type> acquire()
    {
        std::unique_ptr<connection_type> conn(new connection_type(conn_factory_->getConnectionString()));
        return std::move(conn);
    }

    static void init(const std::string& connection_string)
    {
        get_connection_string() = connection_string;
    }
private:
    Manager_(std::unique_ptr<T> _conn_factory)
        : conn_factory_(std::move(_conn_factory))
    { }
    static std::string& get_connection_string()
    {
        static std::string singleton;
        return singleton;
    }
    std::unique_ptr<T> conn_factory_;
};

}//namespace Database

#endif//MANAGER_HH_7B50C0E33D764413851378E6AB696A4E
