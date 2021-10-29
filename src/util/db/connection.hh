/*
 * Copyright (C) 2018-2020  CZ.NIC, z. s. p. o.
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
 *  @file connection.h
 *  Interface definition of automanaged connection object
 */

#ifndef CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24
#define CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24

#include "config.h"

#ifdef HAVE_LOGGER
#include "util/log/log.hh"
#endif

#include "util/db/db_exceptions.hh"
#include "util/db/query_param.hh"
#include "util/db/param_query_composition.hh"

#include <istream>
#include <string>
#include <vector>

namespace Database {

/**
 * \class  Connection_
 * \brief  Standard connection proxy class
 */
template <class connection_driver, class manager_type>
class Connection_
{
public:
    using driver_type = connection_driver;//e.g. PSQLConnection
    using result_type = typename manager_type::result_type;

    explicit Connection_(connection_driver* _conn) : conn_(_conn) { }

    ~Connection_()
    {
        this->close();
    }

    Connection_() = delete;
    Connection_(const Connection_&) = delete;
    Connection_& operator=(const Connection_&) = delete;

    /**
     * Close connection
     */
    void close()
    {
        if (conn_ != nullptr)
        {
            manager_type::connection_factory::release(conn_);
            conn_ = nullptr;
#ifdef HAVE_LOGGER
            try
            {
                FREDLOG_INFO("connection closed");
            }
            catch (...) {}
#endif
        }
    }

    result_type exec(const std::string& _stmt)//throw (ResultFailed)
    {
        this->check_open();
        try
        {
#ifdef HAVE_LOGGER
            FREDLOG_DEBUG(boost::format("exec query [%1%]") % _stmt);
#endif
            return result_type(this->get_opened_connection().exec(_stmt));
        }
        catch (const ResultFailed&)
        {
            throw;
        }
        catch (...)
        {
            throw ResultFailed(_stmt);
        }
    }

    result_type exec_params(
            const std::string& _stmt, //one command query
            const std::vector<std::string>& params)//parameters data
    {
        this->check_open();
        try
        {
#ifdef HAVE_LOGGER
            FREDLOG_DEBUG(boost::format("exec query [%1%]") % _stmt);
#endif
            return result_type(this->get_opened_connection().exec_params(_stmt, //one command query
                                                                         params));//parameters data
        }
        catch (const ResultFailed&)
        {
            throw;
        }
        catch (...)
        {
            throw ResultFailed(_stmt);
        }
    }

    result_type exec_params(const std::string& _stmt, //one command query
                            const QueryParams& params)//parameters data
    {
        this->check_open();
        try
        {
#ifdef HAVE_LOGGER
            std::string value;
            std::string params_dump;
            std::size_t params_counter = 0;
            for (const auto& param : params)
            {
                ++params_counter;
                value = param.is_null() ? "[null]" : "'" + param.print_buffer() + "'";
                params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " + value;
            }
            FREDLOG_DEBUG(boost::format("exec query [%1%] params %2%") % _stmt % params_dump);
#endif
            return result_type(this->get_opened_connection().exec_params(_stmt, //one command query
                                                                         params));//parameters data
        }
        catch (const ResultFailed&)
        {
            throw;
        }
        catch (...)
        {
            throw ResultFailed(_stmt);
        }
    }

    /**
     * ParamQuery wrapper
     * @param _q composable query instance
     * @return result
     */
    result_type exec_params(const ParamQuery& param_query)
    {
        std::pair<std::string, QueryParams> param_query_pair = param_query.get_query();
        return this->exec_params(param_query_pair.first, param_query_pair.second);
    }

    result_type copy_from(std::istream& input_data, const std::string& table_name, std::size_t buffer_size=8192)
    {
        this->check_open();
        try
        {
#ifdef HAVE_LOGGER
            FREDLOG_DEBUG(boost::format{"exec COPY FROM [table=%1%, buffer_size=%2%]"} % table_name % buffer_size);
#endif
            return result_type{this->get_opened_connection().copy_from(input_data, table_name, buffer_size)};
        }
        catch (const ResultFailed&)
        {
            throw;
        }
        catch (...)
        {
            throw ResultFailed{"COPY FROM failed"};
        }
    }

    std::string escape(const std::string& _in)
    {
        return this->get_opened_connection().escape(_in);
    }

    bool is_in_valid_transaction()const
    {
        return (conn_ != nullptr) && conn_->is_in_valid_transaction();
    }

    /**
     * @return  true if there is active transaction on connection
     *          false otherwise
     */
    bool is_in_transaction()const
    {
        return (conn_ != nullptr) && conn_->is_in_transaction();
    }

    /**
     * Reset connection to state after connect
     */
    void reset()
    {
        this->get_opened_connection().reset();
    }

    void setQueryTimeout(unsigned t)
    {
        this->get_opened_connection().setQueryTimeout(t);
#ifdef HAVE_LOGGER
        FREDLOG_DEBUG(boost::format("sql statement timout set to %1%ms") % t);
#endif
    }
private:
    void check_open()const
    {
        if (conn_ == nullptr)
        {
            throw std::runtime_error("Expectation violation - connection is closed");
        }
    }
    connection_driver& get_opened_connection()const
    {
        this->check_open();
        return *conn_;
    }
    connection_driver* conn_; ///< connection_driver instance
};

}//namespace Database

#endif//CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24
