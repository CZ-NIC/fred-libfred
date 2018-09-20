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
 *  @file convert_sql_boost_datetime.hh
 *  Definitions of type conversions for boost datetime types from/to
 *  sql query string.
 */

#ifndef CONVERT_SQL_BOOST_DATETIME_HH_671FBEF800594263836DFC004A69D86F
#define CONVERT_SQL_BOOST_DATETIME_HH_671FBEF800594263836DFC004A69D86F

#include "util/types/convert_sql_base.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/formatters.hpp>

#include <string>

template <>
struct SqlConvert<boost::posix_time::ptime>
{
    static boost::posix_time::ptime from(const std::string& _in)
    {
        try
        {
            return boost::posix_time::time_from_string(_in);
        }
        catch (...)
        {
            throw ConversionError("from sql", "SqlConvert<boost::posix_time::ptime>");
        }
    }

    static std::string to(const boost::posix_time::ptime& _in)
    {
        try
        {
            std::string iso = boost::posix_time::to_iso_extended_string(_in);
            auto i = iso.find("T");
            if (i != std::string::npos)
            {
                iso.replace(i, 1, " ");
            }
            i = 0;
            if ((i = iso.find_last_of(",")) != std::string::npos)
            {
                iso.replace(i, 1, ".");
            }
            return iso;
        }
        catch (...)
        {
            throw ConversionError("to sql", "SqlConvert<boost::posix_time::ptime>");
        }
    }
};

template <>
struct SqlConvert<boost::gregorian::date>
{
    static boost::gregorian::date from(const std::string& _in)
    {
        try
        {
            return boost::gregorian::from_string(_in);
        }
        catch (...)
        {
            throw ConversionError("from sql", "SqlConvert<boost::gregorian::date>");
        }
    }

    static std::string to(const boost::gregorian::date& _in)
    {
        try
        {
            return boost::gregorian::to_iso_extended_string(_in);
        }
        catch (...)
        {
            throw ConversionError("to sql", "SqlConvert<boost::gregorian::date>");
        }
    }
};

#endif//CONVERT_SQL_BOOST_DATETIME_HH_671FBEF800594263836DFC004A69D86F
