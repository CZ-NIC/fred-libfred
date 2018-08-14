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
 *  @file connection.h
 *  Interface definition of automanaged connection object
 */

#ifndef CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24
#define CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24

#include "config.h"

#ifdef HAVE_LOGGER
#include "src/util/log/logger.hh"
#endif

#include "src/util/db/db_exceptions.hh"
#include "src/util/db/query_param.hh"
#include "src/util/db/param_query_composition.hh"

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
    using driver_type = connection_driver;
    using result_type = typename manager_type::result_type;

    Connection_() : conn_(nullptr) { }

    explicit Connection_(
            const std::string& _conn_info,
            bool _lazy_connect = true) //throw (ConnectionFailed)
        : conn_(nullptr),
          conn_info_(_conn_info)
    {
        /* lazy connection open */
        if (!_lazy_connect)
        {
            this->open();
        }
    }

    explicit Connection_(connection_driver* _conn) : conn_(_conn) { }

    /**
     * Destructor
     *
     * close connection on destruct
     */
    ~Connection_()
    {
        this->close();
    }

    /**
     * Open connection with specific connection string
     */
    void open(const std::string& _conn_info) /* throw (ConnectionFailed) */
    {
        conn_info_ = _conn_info;
        this->open();
    }

    /**
     * Close connection
     */
    void close()
    {
        if (conn_ != nullptr)
        {
            delete conn_;
            conn_ = nullptr;
#ifdef HAVE_LOGGER
            try
            {
                LOGGER(PACKAGE).info(boost::format("connection closed; (%1%)") %
                                     connection_driver::get_nopass_conn_info(conn_info_));
            }
            catch (...) {}
#endif
        }
    }

    result_type exec(const std::string& _stmt)//throw (ResultFailed)
    {
        if (conn_ == nullptr)
        {
            this->open();
        }
        try
        {
#ifdef HAVE_LOGGER
            LOGGER(PACKAGE).debug(boost::format("exec query [%1%]") % _stmt);
#endif
            return result_type(conn_->exec(_stmt));
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
            const std::string& _stmt,//one command query
            const std::vector<std::string>& params)//parameters data
    {
        if (conn_ == nullptr)
        {
            this->open();
        }
        try
        {
#ifdef HAVE_LOGGER
            LOGGER(PACKAGE).debug(boost::format("exec query [%1%]") % _stmt);
#endif
            return result_type(conn_->exec_params(_stmt,//one command query
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

    result_type exec_params(const std::string& _stmt,//one command query
                            const QueryParams& params)//parameters data
    {
        if (conn_ == nullptr)
        {
            this->open();
        }
        try
        {
#ifdef HAVE_LOGGER
            if (LOGGER(PACKAGE).getLevel() >= Logging::Log::LL_DEBUG)
            {
                std::string value;
                std::string params_dump;
                std::size_t params_counter = 0;
                for (QueryParams::const_iterator i = params.begin(); i != params.end(); ++i)
                {
                    ++params_counter;
                    value = i->is_null() ? "[null]" : "'" + i->print_buffer() + "'";
                    params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " + value;
                }
                LOGGER(PACKAGE).debug(boost::format("exec query [%1%] params %2%") % _stmt % params_dump);
            }
#endif
            return result_type(conn_->exec_params(_stmt,//one command query
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

    std::string escape(const std::string& _in)
    {
        if (conn_ == nullptr)
        {
            this->open(conn_info_);
        }
        return conn_->escape(_in);
    }

    bool in_valid_transaction()const
    {
        return (conn_ != nullptr) && conn_->in_valid_transaction();
    }

    /**
     * @return  true if there is active transaction on connection
     *          false otherwise
     */
    bool inTransaction()const
    {
        return conn_->inTransaction();
    }

    /**
     * Reset connection to state after connect
     */
    void reset()
    {
        conn_->reset();
    }

    void setConstraintExclusion(bool on = true)
    {
        conn_->setConstraintExclusion(on);
    }

    void setQueryTimeout(unsigned t)
    {
        conn_->setQueryTimeout(t);
#ifdef HAVE_LOGGER
        LOGGER(PACKAGE).debug(boost::format("sql statement timout set to %1%ms") % t);
#endif
    }

/* HACK! HACK! HACK! (use with construct with old DB connection) */
    typename driver_type::__conn_type__ __getConn__()const
    {
        return conn_->__getConn__();
    }
private:
    connection_driver* conn_; ///< connection_driver instance
    std::string conn_info_; ///< connection string used to open connection
    void open()
    {
        this->close();
        // TODO: this should be done by manager_type!
        this->conn_ = new connection_driver(conn_info_);
#ifdef HAVE_LOGGER
        LOGGER(PACKAGE).info(boost::format("connection established; (%1%)") %
                             connection_driver::get_nopass_conn_info(conn_info_));
#endif
    }
};

}//namespace Database

#endif//CONNECTION_HH_6C0F30D7035E4AFDA6456A86D2FD5B24
