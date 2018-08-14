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
 *  @file psql_connection.h
 *  Implementation of connection object for PSQL database.
 */

#ifndef PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC
#define PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC

#include "src/util/db/psql/psql_result.hh"
#include "src/util/db/db_exceptions.hh"

#include <libpq-fe.h>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <algorithm>
#include <memory>
#include <string>

namespace Database {

#ifdef HAVE_LOGGER
static void logger_notice_processor(void*, const char* message)
{
    // replace new line symbols
    std::string msg(message);
    std::replace(msg.begin(), msg.end(), '\n', ' ');
    LOGGER(PACKAGE).debug(msg);
}
#endif

class PSQLTransaction;

/**
 * \class PSQLConnection
 * \brief PSQL connection driver for Connection_ template
 */
class PSQLConnection
{
public:
    typedef PSQLResult result_type;
    typedef PSQLTransaction transaction_type;

    /**
     * Constructors and destructor
     */
    PSQLConnection()
        : psql_conn_(nullptr),
          psql_conn_finish_(true)
    { }

    PSQLConnection(const std::string& _conn_info) /* throw (ConnectionFailed) */
        : conn_info_(_conn_info),
          psql_conn_(nullptr),
          psql_conn_finish_(true)
    {
        this->open(_conn_info);
    }

    PSQLConnection(PGconn* _psql_conn)
        : psql_conn_(_psql_conn),
          psql_conn_finish_(false)
    { }

    ~PSQLConnection()
    {
        close();
    }

    static std::string get_nopass_conn_info(const std::string& conn_info)
    {
        static const boost::regex re_find_password("password=[^\\ ]+");
        return boost::regex_replace(conn_info, re_find_password, "password=******** ");
    }

    /**
     * String which matches the message in database exception if
     * timeout (set by setQueryTimeout) occurs
     * it's too hard to implement it as const member in this class and in ConnectionBase_
     */
    static std::string getTimeoutString()
    {
        return std::string("statement timeout");
    }

    /**
     * Implementation of corresponding methods called by Connection_ template
     */
    void open(const std::string& _conn_info) /* throw (ConnectionFailed) */
    {
        conn_info_ = _conn_info;
        close();
        psql_conn_ = PQconnectdb(_conn_info.c_str());
        if (PQstatus(psql_conn_) != CONNECTION_OK)
        {
            const std::string err_msg =  PQerrorMessage(psql_conn_);
            PQfinish(psql_conn_);
            throw ConnectionFailed(_conn_info + " errmsg: " + err_msg);
        }
#ifdef HAVE_LOGGER
        // set notice processor
        PQsetNoticeProcessor(psql_conn_, logger_notice_processor, NULL);
#endif
    }

    void close()
    {
        if (psql_conn_ && psql_conn_finish_)
        {
            PQfinish(psql_conn_);
            psql_conn_ = nullptr;
        }
    }

    result_type exec(const std::string& _query) /*throw (ResultFailed)*/
    {
        PGresult* const tmp = PQexec(psql_conn_, _query.c_str());

        const ExecStatusType status = PQresultStatus(tmp);
        if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
        {
            return PSQLResult(tmp);
        }
        PQclear(tmp);
        throw ResultFailed(_query + " (" + PQerrorMessage(psql_conn_) + ")");
    }

    result_type exec_params(
            const std::string& _query,//one command query
            const std::vector<std::string>& params)//parameters data
    {
        std::vector< const char * > paramValues; //pointer to memory with parameters data
        std::vector<int> paramLengths; //sizes of memory with parameters data

        for (std::vector< std::string>::const_iterator i = params.begin(); i != params.end() ; ++i)
        {
            paramValues.push_back((*i).c_str());
            paramLengths.push_back((*i).size());
        }

        PGresult* const tmp = PQexecParams(
                psql_conn_,
                _query.c_str(),//query buffer
                paramValues.size(),//number of parameters
                0,//not using Oids, use type in query like: WHERE id = $1::int4 and name = $2::varchar
                &paramValues[0],//values to substitute $1 ... $n
                &paramLengths[0],//the lengths, in bytes, of each of the parameter values
                0,//param values are strings
                0);//we want the result in text format

        const ExecStatusType status = PQresultStatus(tmp);
        if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
        {
            return PSQLResult(tmp);
        }
        PQclear(tmp);
        std::string params_dump;
        std::size_t params_counter = 0;
        for (std::vector<std::string>::const_iterator i = params.begin(); i != params.end() ; ++i)
        {
            ++params_counter;
            params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " + *i;
        }//for params

        throw ResultFailed("query: " + _query + " "
                           "Params:" + params_dump + " "
                           "(" + PQerrorMessage(psql_conn_) + ")");
    }

    result_type exec_params(
            const std::string& _query,//one command query
            const QueryParams& params)//parameters data
    {
        const Oid BYTEAOID = 17;//binary param type

        std::vector<Oid> paramTypes;//types of query parameters
        std::vector<const char*> paramValues; //pointer to memory with parameters data
        std::vector<int> paramLengths; //sizes of memory with parameters data
        std::vector<int> paramFormats; //format of parameter data

        for (QueryParams::const_iterator i = params.begin(); i != params.end() ; ++i)
        {
            paramTypes.push_back(i->is_binary() ? BYTEAOID : 0);
            paramValues.push_back(i->is_null() ? 0 : &(i->get_data())[0] );
            paramLengths.push_back(i->get_data().size());
            paramFormats.push_back(i->is_binary() ? 1 : 0 );
        }

        PGresult* const tmp = PQexecParams(
                psql_conn_,
                _query.c_str(),//query buffer
                paramValues.size(),//number of parameters
                paramTypes.size() != 0 ? &paramTypes[0] : nullptr,
                &paramValues[0],//values to substitute $1 ... $n
                &paramLengths[0],//the lengths, in bytes, of each of the parameter values
                &paramFormats[0],//param values formats
                0);//we want the result in text format

        const ExecStatusType status = PQresultStatus(tmp);
        if ((status == PGRES_COMMAND_OK) || (status == PGRES_TUPLES_OK))
        {
            return PSQLResult(tmp);
        }
        PQclear(tmp);

        std::string params_dump;
        std::size_t params_counter =0;

        for (QueryParams::const_iterator i = params.begin(); i != params.end() ; ++i)
        {
            ++params_counter;
            params_dump += " $" + boost::lexical_cast<std::string>(params_counter) + ": " +
                           (i->is_null() ? std::string("null")
                                         : (i->is_binary() ? std::string("binary") : i->get_data()));
        }

        throw ResultFailed("query: " + _query + " "
                           "Params:" + params_dump + " (" + PQerrorMessage(psql_conn_) + ")");
    }

    void setConstraintExclusion(bool on = true)
    {
        if (on)
        {
            this->exec("SET constraint_exclusion=ON");
        }
        else
        {
            this->exec("SET constraint_exclusion=OFF");
        }
    }

    void setQueryTimeout(unsigned t)
    {
        boost::format fmt_timeout = boost::format("SET statement_timeout=%1%") % t;
        exec(fmt_timeout.str());
    }

    void reset()
    {
        PQreset(psql_conn_);
    }

    std::string escape(const std::string& _in)const
    {
        std::string ret;
        int err;
        {
            const std::unique_ptr<char[]> esc(new char[(2 * _in.size()) + 1]);
            PQescapeStringConn(psql_conn_, esc.get(), _in.c_str(), _in.size(), &err);
            ret = esc.get();
        }
        if (err != 0)
        {
            /* error */
            const std::string msg = str(boost::format("error in escape function: %1%") % PQerrorMessage(psql_conn_));
#ifdef HAVE_LOGGER
            LOGGER(PACKAGE).error(msg);
#endif
            throw std::runtime_error(msg);
        }
        return ret;
    }

    bool inTransaction()const
    {
        switch (PQtransactionStatus(psql_conn_))
        {
            case PQTRANS_INTRANS://idle, within transaction block
            case PQTRANS_INERROR://idle, within failed transaction
                return true;
            case PQTRANS_IDLE:   //connection idle
            case PQTRANS_ACTIVE: //command in progress
            case PQTRANS_UNKNOWN://cannot determine status
                return false;
        }
        throw std::runtime_error("PQtransactionStatus() failure: unexpected return value");
    }

    bool in_valid_transaction()const
    {
        switch (PQtransactionStatus(psql_conn_))
        {
            case PQTRANS_INTRANS://idle, within transaction block
                return true;
            case PQTRANS_INERROR://idle, within failed transaction
            case PQTRANS_IDLE:   //connection idle
            case PQTRANS_ACTIVE: //command in progress
            case PQTRANS_UNKNOWN://cannot determine status
                return false;
        }
        throw std::runtime_error("PQtransactionStatus() failure: unexpected return value");
    }

    /* HACK! HACK! HACK! */
    typedef PGconn* __conn_type__;
    __conn_type__ __getConn__()const
    {
        return psql_conn_;
    }
private:
    std::string conn_info_;
    PGconn* psql_conn_; ///< wrapped connection structure from libpq library
    bool psql_conn_finish_; ///< whether or not to finish PGconn at the destruction (close() method)
};

/**
 * \class PSQLTransaction
 * \brief Implementation of local transaction for PSQL driver
 */
class PSQLTransaction
{
public:
    typedef PSQLConnection connection_type;

    PSQLTransaction() { }

    ~PSQLTransaction() { }

    static std::string start()
    {
        return "START TRANSACTION ISOLATION LEVEL READ COMMITTED";
    }

    static std::string rollback()
    {
        return "ROLLBACK TRANSACTION";
    }

    static std::string commit()
    {
        return "COMMIT TRANSACTION";
    }

    static std::string prepare(const std::string& _id)
    {
        return "PREPARE TRANSACTION '" + _id + "'";
    }
};

}//namespace Database

#endif//PSQL_CONNECTION_HH_7365F969C663447FA2B3C2A0FAEE30CC
