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
 *  @file manager.hh
 *  Manager_ for database connections.
 */

#ifndef MANAGER_HH_7B50C0E33D764413851378E6AB696A4E
#define MANAGER_HH_7B50C0E33D764413851378E6AB696A4E

#include "config.h"

#include "util/db/result.hh"
#include "util/db/connection.hh"
#include "util/db/transaction.hh"

#ifdef HAVE_LOGGER
#include "util/log/logger.hh"
#endif

#include <atomic>
#include <memory>
#include <stdexcept>

namespace Database {

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
//
// Note:
//
//     class Connection_<PSQLConnection, ...>
//     {
//         PSQLConnection* conn_;
//     };
//
//     class Factory::Simple<PSQLConnection>
//     {
//         PSQLConnection* acquire()const
//         {
//             return new PSQLConnection(...);
//         }
//     };
//
//     class Manager<Factory::Simple<PSQLConnection>>
//     {
//         Connection_<PSQLConnection, ...>* acquire()const
//         {
//             return new Connection_<PSQLConnection, ...>(conn_factory_.acquire())
//         }
//         Factory::Simple<PSQLConnection> conn_factory_;
//     };
//
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

/**
 * \class Manager_
 * \brief Simple database manager
 *
 * This object populated with specific connection factory can be used as
 * connection manager
 */
template <class T>// e.g. Factory::Simple<PSQLConnection>
class Manager_
{
public:
    using connection_factory = T;// e.g. Factory::Simple<PSQLConnection>
    using connection_driver = typename T::connection_driver;// e.g. PSQLConnection
    using connection_type = Connection_<connection_driver, Manager_>;// e.g. Connection<PSQLConnection, Manager<Factory<PSQLConnection>>
    using result_type = Result_<typename connection_driver::ResultType>;
    using row_type = typename result_type::Row;

    template <typename ...A>
    Manager_(A ...args)
        : conn_factory_(args...)
    { }

    ~Manager_() = default;

    std::unique_ptr<connection_type> acquire()const
    {
        std::unique_ptr<connection_type> conn(new connection_type(conn_factory_.acquire()));
        return std::move(conn);
    }
private:
    connection_factory conn_factory_;
};

template <typename T>
void set_default_manager(std::unique_ptr<Manager_<T>> default_manager);

template <typename T>
const Manager_<T>& get_default_manager();

template <typename T>
class DefaultManagerProvider
{
private:
    template <typename F>
    friend void set_default_manager(std::unique_ptr<Manager_<F>> default_manager);
    template <typename F>
    friend const Manager_<F>& get_default_manager();
    static std::unique_ptr<Manager_<T>>* storage_ptr_;
};

template <typename T>
std::unique_ptr<Manager_<T>>* DefaultManagerProvider<T>::storage_ptr_ = nullptr;

template <typename T>
const Manager_<T>& get_default_manager()
{
    if (DefaultManagerProvider<T>::storage_ptr_ != nullptr)
    {
        return **DefaultManagerProvider<T>::storage_ptr_;
    }
    throw std::runtime_error("Uninitialized default database manager");
}

template <typename T>
void set_default_manager(std::unique_ptr<Manager_<T>> default_manager)
{
    if (DefaultManagerProvider<T>::storage_ptr_ != nullptr)
    {
        throw std::runtime_error("Default database manager was initialized");
    }
    static std::atomic<bool> lock(false);
    bool expected_lock = false;
    if (!lock.compare_exchange_weak(expected_lock, true, std::memory_order_release, std::memory_order_relaxed))
    {
        throw std::runtime_error("Default database manager was initialized");
    }
    static std::unique_ptr<Manager_<T>> singleton(std::move(default_manager));
    DefaultManagerProvider<T>::storage_ptr_ = &singleton;
}

template <typename M, typename ...A>
void emplace_default_manager(A ...args)
{
    set_default_manager(std::make_unique<M>(args...));
}

}//namespace Database

#endif//MANAGER_HH_7B50C0E33D764413851378E6AB696A4E
