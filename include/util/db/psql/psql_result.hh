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
 *  @file psql_result.hh
 *  Implementation of result object for PSQL database.
 */

#ifndef PSQL_RESULT_HH_B4F0DE7B238A4A23B6F7C82EC3692B2F
#define PSQL_RESULT_HH_B4F0DE7B238A4A23B6F7C82EC3692B2F

#include "util/db/row.hh"
#include "util/db/value.hh"
#include "util/db/result.hh"

#include <libpq-fe.h>

#include <string>
#include <iterator>
#include <memory>

namespace Database {

class PSQLConnection;

/**
 * \class PSQLResult
 * \brief PSQL result object
 */
class PSQLResult
{
public:
    using size_type = unsigned;
    using value_type = Value;
    using Row = Row_<PSQLResult, value_type>;

    PSQLResult();

    PSQLResult(const PSQLResult&);

    ~PSQLResult();

    void clear();

    /**
     * Implementation of corresponding methods called by Result_ template
     */
    size_type size()const;

    /**
     * Number of rows affected by non-select query
     */
    size_type rows_affected()const;

    /**
     * Iterator interface definition
     */
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, Row>
    {
    public:
        Iterator(const PSQLResult* _data_ptr, unsigned _row = 0);

        Iterator(const Iterator&);

        value_type operator*()const;

        Iterator& operator+(int n);

        Iterator& operator++();

        Iterator& operator+=(int n);

        Iterator& operator-(int n);

        Iterator& operator--();

        bool operator==(const Iterator&)const;

        bool operator!=(const Iterator&)const;
    private:
        const PSQLResult* data_ptr_;
        unsigned row_;
    };

    Iterator begin()const;

    Iterator end()const;
private:
    PSQLResult(const std::shared_ptr<PGresult>& _psql_result);

    /**
     * @return number of columns
     */
    size_type cols_()const;

    /**
     * @return number of rows
     */
    size_type rows_()const;

    /**
     * @param  row_idx row number
     * @param  col_idx column number
     */
    void check_range(size_type row_idx, size_type col_idx)const;// throw(OutOfRange)

    /**
     * @param  _r row number
     * @param  _c column number
     * @return    value from result at position [_r, _c]
     */
    std::string value_(size_type _r, size_type _c)const;// throw(OutOfRange)

    /**
     * @param  _r row number
     * @param  _c column number
     * @return    true if value from result at position [_r, _c] is null, false otherwise
     */
    bool value_is_null_(size_type _r, size_type _c)const;// throw(OutOfRange)

    /**
     * @param  _c column name
     * @return column index
     */
    int get_column_number(const std::string& column_name)const;// throw(NoSuchField)

    /**
     * @param row_idx row number
     * @param column_name column name
     * @return value from result at position [row_idx, column_name]
     */
    std::string value_(size_type row_idx, const std::string& column_name)const;// throw(NoSuchField)

    /**
     * @param row_idx row number
     * @param column_name column name
     * @return true if value from result at position [row_idx, column_name] is null, false otherwise
     */
    bool value_is_null_(size_type row_idx, const std::string& column_name)const;// throw(NoSuchField)
    friend class Row_<PSQLResult, value_type>;
    friend class Row_<PSQLResult, value_type>::Iterator;
    friend class Result_<PSQLResult>;
    friend class PSQLConnection;
    std::shared_ptr<PGresult> psql_result_;///< wrapped result structure from lipq library
};

}//namespace Database

#endif//PSQL_RESULT_HH_B4F0DE7B238A4A23B6F7C82EC3692B2F
