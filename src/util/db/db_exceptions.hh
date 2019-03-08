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
#ifndef DB_EXCEPTIONS_HH_059C8712324C43CBB7CCBBDC0B847D3C
#define DB_EXCEPTIONS_HH_059C8712324C43CBB7CCBBDC0B847D3C

#include "util/base_exception.hh"

#include <sstream>

namespace Database {

class Exception : public ::Exception
{
public:
    Exception(const std::string& _what) : ::Exception(_what) { }
};

class ConnectionFailed : public Database::Exception
{
public:
    ConnectionFailed(const std::string& _conn_info) : Exception("Connection failed: " + _conn_info) { }
};

class ResultFailed : public Database::Exception
{
public:
    ResultFailed(const std::string& _query) : Exception("Result failed: " + _query) { }
};

class OutOfRange : public Database::Exception
{
public:
    template <typename T>
    struct BeginEnd
    {
        BeginEnd(T _begin, T _end) : begin(_begin), end(_end) { }
        const T begin;
        const T end;
    };
    template <typename T>
    OutOfRange(T given, const BeginEnd<T>& range) : Exception(make_message(given, range)) { }
private:
    template <typename T>
    static std::string make_message(T given, const BeginEnd<T>& range)
    {
        std::ostringstream msg;
        msg << "Out of range: field column " << given << " is out of range [" << range.begin << ".." << range.end << ")";
        return msg.str();
    }
};

class NoSuchField : public Database::Exception
{
public:
    NoSuchField(const std::string& _name) : Exception("No such field name: '" + _name + "'") { }
};

class NoDataFound : public Database::Exception
{
public:
    NoDataFound(const std::string& _reason) : Exception("No data found: " + _reason) { }
};

}//namespace Database

#endif//DB_EXCEPTIONS_HH_059C8712324C43CBB7CCBBDC0B847D3C
