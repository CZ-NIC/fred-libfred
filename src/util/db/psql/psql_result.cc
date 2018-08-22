/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
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
 *  @file psql_result.cc
 *  Implementation of result object for PSQL database.
 */

#include "util/db/psql/psql_result.hh"
#include "util/db/db_exceptions.hh"

#include <boost/lexical_cast.hpp>

#include <cstdlib>
#include <string>
#include <iterator>
#include <memory>

namespace Database {

PSQLResult::PSQLResult()
    : psql_result_(nullptr)
{ }

PSQLResult::PSQLResult(const PSQLResult& _other)
    : psql_result_(_other.psql_result_)
{ }

PSQLResult::~PSQLResult()
{ }

void PSQLResult::clear()
{
    psql_result_ = nullptr;
}

PSQLResult::size_type PSQLResult::size()const
{
    return PQntuples(psql_result_.get());
}

PSQLResult::size_type PSQLResult::rows_affected()const
{
    const std::string number_in_string(PQcmdTuples(psql_result_.get()));
    const size_type ret = boost::lexical_cast<size_type>(number_in_string);
    return ret;
}

PSQLResult::Iterator::Iterator(const PSQLResult* _data_ptr, unsigned _row)
    : data_ptr_(_data_ptr),
      row_(_row)
{ }

PSQLResult::Iterator::Iterator(const Iterator& _other)
{
    data_ptr_ = _other.data_ptr_;
    row_ = _other.row_;
}

PSQLResult::Iterator::value_type PSQLResult::Iterator::operator*()const
{
    return value_type(data_ptr_, row_);
}

PSQLResult::Iterator& PSQLResult::Iterator::operator+(int _n)
{
    row_ += _n;
    return *this;
}

PSQLResult::Iterator& PSQLResult::Iterator::operator++()
{
    ++row_;
    return *this;
}

PSQLResult::Iterator& PSQLResult::Iterator::operator+=(int _n)
{
    row_ += _n;
    return *this;
}

PSQLResult::Iterator& PSQLResult::Iterator::operator-(int _n)
{
    row_ -= _n;
    return *this;
}

PSQLResult::Iterator& PSQLResult::Iterator::operator--()
{
    --row_;
    return *this;
}

bool PSQLResult::Iterator::operator==(const Iterator& _other)const
{
    return (data_ptr_ == _other.data_ptr_) && (row_ == _other.row_);
}

bool PSQLResult::Iterator::operator!=(const Iterator& _other)const
{
    return !(*this == _other);
}

PSQLResult::Iterator PSQLResult::begin()const
{
    return Iterator(this);
}

PSQLResult::Iterator PSQLResult::end()const
{
    return Iterator(this, size());
}

PSQLResult::PSQLResult(const std::shared_ptr<PGresult>& _psql_result)
    : psql_result_(_psql_result)
{ }

/**
 * @return number of columns
 */
PSQLResult::size_type PSQLResult::cols_()const
{
    return PQnfields(psql_result_.get());
}

/**
 * @return number of rows
 */
PSQLResult::size_type PSQLResult::rows_()const
{
    return PQntuples(psql_result_.get());
}

namespace {

template <typename T>
void check_increasing_sequence(T a0, T a1, T a2)
{
    if ((a0 <= a1) && (a1 < a2))
    {
        return;
    }
    throw OutOfRange(a1, {a0, a2});
}

}//namespace Database::{anonymous}

/**
 * @param  row_idx row number
 * @param  col_idx column number
 */
void PSQLResult::check_range(size_type row_idx, size_type col_idx)const
{
    check_increasing_sequence(0u, row_idx, this->rows_());
    check_increasing_sequence(0u, col_idx, this->cols_());
}

/**
 * @param  _r row number
 * @param  _c column number
 * @return    value from result at position [_r, _c]
 */
std::string PSQLResult::value_(size_type _r, size_type _c)const
{
    this->check_range(_r, _c);
    return PQgetvalue(psql_result_.get(), _r, _c);
}

/**
 * @param  _r row number
 * @param  _c column number
 * @return    true if value from result at position [_r, _c] is null, false otherwise
 */
bool PSQLResult::value_is_null_(size_type _r, size_type _c)const
{
    this->check_range(_r, _c);
    return PQgetisnull(psql_result_.get(), _r, _c);
}

/**
 * @param  _c column name
 * @return column index
 */
int PSQLResult::get_column_number(const std::string& column_name)const
{
    const int column_number = PQfnumber(psql_result_.get(), column_name.c_str());
    constexpr int no_such_field = -1;
    if (column_number == no_such_field)
    {
        throw NoSuchField(column_name);
    }
    return column_number;
}

/**
 * @param row_idx row number
 * @param column_name column name
 * @return value from result at position [row_idx, column_name]
 */
std::string PSQLResult::value_(size_type row_idx, const std::string& column_name)const
{
    check_increasing_sequence(0u, row_idx, this->rows_());
    return PQgetvalue(psql_result_.get(), row_idx, this->get_column_number(column_name));
}

/**
 * @param row_idx row number
 * @param column_name column name
 * @return true if value from result at position [row_idx, column_name] is null, false otherwise
 */
bool PSQLResult::value_is_null_(size_type row_idx, const std::string& column_name)const
{
    check_increasing_sequence(0u, row_idx, this->rows_());
    return PQgetisnull(psql_result_.get(), row_idx, this->get_column_number(column_name));
}

}//namespace Database
