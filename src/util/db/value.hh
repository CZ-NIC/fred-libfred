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
 *  @file value.hh
 *  Implementation of value object.
 */

#ifndef VALUE_HH_345838BAA73C4BD3ACC134711E9B1E05
#define VALUE_HH_345838BAA73C4BD3ACC134711E9B1E05

#include "util/util.hh"
#include "util/log/log.hh"
#include "util/types/data_types.hh"
#include "util/types/convert_sql_pod.hh"
#include "util/types/convert_sql_boost_datetime.hh"
#include "util/types/convert_sql_db_types.hh"
#include "util/types/convert_sql_std_chrono_types.hh"
#include "util/types/sqlize.hh"

#include <boost/function.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <chrono>
#include <string>

/**
 * Macros for defining constructors, cast operators and assignments operator
 * from other types to string.
 *
 * Can be use only for simple conversions - calling function or method on
 * appropriate type. Special handling must be done manually.
 */
#define CONSTRUCTOR(_type, _quoted, _escaped)                \
Value(const _type& _value) : is_null_(false),                \
                             value_(sqlize(_value)),         \
                             quoted_output_(_quoted),        \
                             escaped_output_(_escaped)       \
{                                                            \
}

#define ASSIGN_OPERATOR(_type, _quoted, _escaped)            \
Value& operator=(const _type& _value)                        \
{                                                            \
    value_ = sqlize(_value);                                 \
    is_null_ = false;                                        \
    quoted_output_ = _quoted;                                \
    escaped_output_ = _escaped;                              \
    return *this;                                            \
}

#define CAST_OPERATOR(_type, _init)                          \
operator _type()const                                        \
{                                                            \
    return is_null_ ? _init : unsqlize<_type>(value_);       \
}

#define HANDLE_TYPE(_type, _init, _quoted, _escaped)         \
CONSTRUCTOR(_type, _quoted, _escaped)                        \
ASSIGN_OPERATOR(_type, _quoted, _escaped)                    \
CAST_OPERATOR(_type, _init)

namespace Database {

/**
 * \class Value
 * \brief Value object is return type from Database::Row container
 *
 * It contains all needed contructors and conversion operators
 * for supported types to simplify getting/setting data from/to SQL
 * result/query
 */
class Value
{
public:
    /**
     * default constructor for NULL value
     */
    Value()
        : is_null_(true),
          value_(),
          quoted_output_(false),
          escaped_output_(false)
    { }

    /**
     * constructor for database result object
     */
    Value(const std::string& _value, bool _is_null)
        : is_null_(_is_null),
          value_(_value),
          quoted_output_(!_is_null),
          escaped_output_(quoted_output_)
    { }

    /**
     * full parameter constructor
     */
    Value(const std::string& _value,
          bool _is_null,
          bool _quoted,
          bool _escaped)
        : is_null_(_is_null),
          value_(_value),
          quoted_output_(_quoted),
          escaped_output_(_escaped)
    { }

    /**
     * construct and conversion operator for string values
     */
    Value(const std::string& _value)
        : is_null_(false),
          value_(_value),
          quoted_output_(true),
          escaped_output_(true)
    { }

    Value(const char* _value)
        : is_null_(false),
          value_(_value),
          quoted_output_(true),
          escaped_output_(true)
    { }

    operator std::string()const
    {
        return (is_null_ ? "" : value_);
    }

    Value& operator=(const std::string& _value)
    {
        value_ = _value;
        is_null_ = false;
        quoted_output_ = true;
        escaped_output_ = true;
        return *this;
    }

    /**
     * Database::ID need special handling
     */
    Value(const Database::ID& _value)
        : is_null_(_value == 0),// do extra checking for NULL (zero value is not valid ID)
          value_(sqlize(_value)),
          quoted_output_(false),
          escaped_output_(false)
    { }

    CAST_OPERATOR(Database::ID, Database::ID())

    /**
     * boost::posix_time::ptime need special handling
     */
    Value(const boost::posix_time::ptime& _value)
    {
        this->__init_date_time(_value);
    }

    CAST_OPERATOR(boost::posix_time::ptime, boost::posix_time::ptime())

    Value& operator=(const boost::posix_time::ptime& _value)
    {
        this->__init_date_time(_value);
        return *this;
    }

    /**
     * boost::date need special handling
     */
    Value(const boost::gregorian::date& _value)
    {
        this->__init_date_time(_value);
    }

    CAST_OPERATOR(boost::gregorian::date, boost::gregorian::date())

    Value& operator=(const boost::gregorian::date& _value)
    {
        this->__init_date_time(_value);
        return *this;
    }

    /**
     * DateTime need special handling
     */
    Value(const DateTime& _value)
    {
        this->__init_date_time(_value);
    }

    CAST_OPERATOR(DateTime, DateTime())

    Value& operator=(const DateTime& _value)
    {
        this->__init_date_time(_value);
        return *this;
    }

    /**
    * Date need special handling
    */
    Value(const Date& _value)
    {
        this->__init_date_time(_value);
    }

    CAST_OPERATOR(Date, Date())

    Value& operator=(const Date& _value)
    {
        this->__init_date_time(_value);
        return *this;
    }

    HANDLE_TYPE(short,              0, false, false)
    HANDLE_TYPE(int,                0, false, false)
    HANDLE_TYPE(long,               0, false, false)
    HANDLE_TYPE(long long,          0, false, false)
    HANDLE_TYPE(unsigned,           0, false, false)
    HANDLE_TYPE(unsigned long,      0, false, false)
    HANDLE_TYPE(unsigned long long, 0, false, false)
    HANDLE_TYPE(float,              0, false, false)
    HANDLE_TYPE(bool,               0, true,  false)

    //  HANDLE_TYPE(DateTime,           DateTime(), true,  false)
    //  HANDLE_TYPE(Date,               Date(),     true,  false)

    template <typename R>
    operator std::chrono::time_point<std::chrono::system_clock, R>()const
    {
        using Type = std::chrono::time_point<std::chrono::system_clock, R>;
        if (this->isnull())
        {
            throw ConversionError("to", "std::chrono::time_point<std::chrono::system_clock, R>");
        }
        return unsqlize<Type>(value_);
    }

    /* assigment */
    Value& operator=(const Value& _other)
    {
        is_null_ = _other.is_null_;
        value_ = _other.value_;
        quoted_output_ = _other.quoted_output_;
        escaped_output_ = _other.escaped_output_;
        return *this;
    }

    /**
     * @return  flag if this value should be quoted in SQL statement or not
     */
    bool quoted()const
    {
        return quoted_output_;
    }

    const std::string& str()const
    {
        return value_;
    }

    /**
     * @return  whether is value null or not
     */
    bool isnull()const
    {
        return is_null_;
    }

    bool operator!()const
    {
        return this->isnull();
    }

    /**
     * to sql string serialization
     *
     * @param _esc_func  string escape function pointer
     */
    std::string toSql(boost::function<std::string(std::string)> _esc_func)const
    {
        if (is_null_)
        {
            return "NULL";
        }
        return (quoted_output_ ? (escaped_output_ ? "E'" : "'") + _esc_func(value_) + "'" : value_);
    }

    template <typename T>
    T as()const
    {
        return static_cast<T>(*this);
    }
protected:
    bool        is_null_;        /**< flag if value is NULL */
    std::string value_;          /**< value in std::string representation */
    bool        quoted_output_;  /**< SQL value quotation flag */
    bool        escaped_output_; /**< SQL value escape flag */
private:
    /**
    * common initialization code for boost ptime
    * and boost date types
    */
    template <class T>
    void __init_date_time(const T& _value)
    {
        escaped_output_ = false;
        if (_value.is_special())
        {
            is_null_ = true;
            quoted_output_ = false;
        }
        else
        {
            value_ = sqlize(_value);
            is_null_ = false;
            quoted_output_ = true;
        }
    }
    friend std::ostream& operator<<(std::ostream& _os, const Value& _value)
    {
        return _value.is_null_ ? _os << "NULL" : _os << _value.toSql(&Util::escape2);
    }
};

std::vector<std::string> array_to_vector(std::string _dbarr);

}//namespace Database

#endif//VALUE_HH_345838BAA73C4BD3ACC134711E9B1E05
