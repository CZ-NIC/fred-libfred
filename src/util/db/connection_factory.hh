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
 *  @file connection_factory.h
 *
 *  Factories defines how connection is acquired and released.
 *  Definition of simple factory and connection pool factory follows.
 */


#ifndef CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257
#define CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257

#include "config.h"

#ifdef HAVE_LOGGER
#include "src/util/log/logger.hh"
#endif

#include <boost/format.hpp>

#include <string>

namespace Database {
namespace Factory {

/**
 * \class  Simple
 * \brief  Very simple factory using constructor and destructor of
 *         connection_driver type
 */
template <class T>
class Simple
{
public:
    using connection_driver = T;

    /**
     * Constuctors and destructor
     */
    explicit Simple(const std::string& _conn_info)
        : conn_info_(_conn_info)
    {
#ifdef HAVE_LOGGER
        TRACE(boost::format("<CALL> Database::Factory::Simple::Simple('%1%')") % this->get_nopass_conn_info());
#endif
    }

    ~Simple()
    {
#ifdef HAVE_LOGGER
        TRACE("<CALL> Database::Factory::Simple::~Simple()");
#endif
    }

    /**
     * Connection factory method
     *
     * @return  connection
     */
    connection_driver* acquire()
    {
        return new connection_driver(conn_info_);
    }

    /**
     * Simple connection releaser - delete it
     *
     * @param _conn  connection pointer
     */
    void release(connection_driver*& _conn)
    {
        if (_conn != nullptr)
        {
            delete _conn;
            _conn = nullptr;
        }
    }

    /**
     * Connection string getter
     *
     * @return  connection string factory was configured with
     */
    const std::string& getConnectionString()const
    {
        return conn_info_;
    }
private:
    /**
     * filter out password from database connection string
     */
    std::string get_nopass_conn_info()const
    {
        return T::get_nopass_conn_info(conn_info_);
    }
    std::string conn_info_; /**< connection string (host, database name, user, password ...) */
};

}//namespace Database::Factory
}//namespace Database

#endif//CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257
