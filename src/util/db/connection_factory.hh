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
 *  @file connection_factory.h
 *
 *  Factories defines how connection is acquired and released.
 *  Definition of simple factory and connection pool factory follows.
 */


#ifndef CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257
#define CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257

#include "config.h"

#ifdef HAVE_LOGGER
#include "util/log/log.hh"
#endif

#include <boost/format.hpp>

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
    using connection_driver = T;//PSQLConnection

    template <typename ...A>
    Simple(A ...args)
        : need_to_open_(args...)
    {
#ifdef HAVE_LOGGER
        FREDLOG_TRACE(boost::format("<CALL> Database::Factory::Simple::Simple('%1%')") %
                  T::to_publicable_string(need_to_open_));
#endif
    }

    ~Simple()
    {
#ifdef HAVE_LOGGER
        FREDLOG_TRACE("<CALL> Database::Factory::Simple::~Simple()");
#endif
    }

    /**
     * Connection factory method
     *
     * @return  connection
     */
    connection_driver* acquire()const
    {
        return new connection_driver(need_to_open_);
    }

    /**
     * Simple connection releaser - delete it
     *
     * @param _conn  connection pointer
     */
    static void release(connection_driver*& _conn)
    {
        if (_conn != nullptr)
        {
            delete _conn;
            _conn = nullptr;
        }
    }
private:
    typename connection_driver::OpenType need_to_open_;
};

}//namespace Database::Factory
}//namespace Database

#endif//CONNECTION_FACTORY_HH_BC10578287C841BC838AF32067FF7257
