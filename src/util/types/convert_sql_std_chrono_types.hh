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
 *  @file convert_sql_chrono_types.hh
 *  Definitions of type conversions for boost datetime types from/to
 *  sql query string.
 */

#ifndef CONVERT_SQL_STD_CHRONO_TYPES_HH_147717D9BC013CC62D1C5A4692408CCF
#define CONVERT_SQL_STD_CHRONO_TYPES_HH_147717D9BC013CC62D1C5A4692408CCF

#include "util/types/convert_sql_boost_datetime.hh"

#include <chrono>
#include <string>

template <typename R>
struct SqlConvert<std::chrono::time_point<std::chrono::system_clock, R>>
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, R>;

    static boost::posix_time::ptime convert_from_time_point(const TimePoint& from)
    {
        const auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(from.time_since_epoch()).count();
        const auto sec_part = nanoseconds / 1000000000;
        const auto nsec_part = nanoseconds % 1000000000;
        return boost::posix_time::from_time_t(0) +
               boost::posix_time::seconds(static_cast<long>(sec_part)) +
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
               boost::posix_time::nanoseconds(nsec_part);
#else
               boost::posix_time::microseconds((nsec_part + 500) / 1000);
#endif
    }

    static TimePoint convert_to_time_point(const boost::posix_time::ptime& from)
    {
        const boost::posix_time::time_duration time_since_epoch = from - boost::posix_time::from_time_t(0);
        const auto sec_part = std::chrono::system_clock::from_time_t(time_since_epoch.total_seconds());
        const auto nsec_part = time_since_epoch.fractional_seconds() * (1000000000 / time_since_epoch.ticks_per_second());
        return sec_part + std::chrono::nanoseconds(nsec_part);

    }

    static TimePoint from(const std::string& src)
    {
        try
        {
            if (src == "-infinity")
            {
                return TimePoint::min();
            }
            if (src == "infinity")
            {
                return TimePoint::max();
            }
            return convert_to_time_point(SqlConvert<boost::posix_time::ptime>::from(src));
        }
        catch (...)
        {
            throw ConversionError("from sql", "SqlConvert<std::chrono::time_point<std::chrono::system_clock, R>>");
        }
    }

    static std::string to(const TimePoint& src)
    {
        try
        {
            if (src == TimePoint::min())
            {
                return "-infinity";
            }
            if (src == TimePoint::max())
            {
                return "infinity";
            }
            return SqlConvert<boost::posix_time::ptime>::to(convert_from_time_point(src));
        }
        catch (...)
        {
            throw ConversionError("to sql", "SqlConvert<std::chrono::time_point<std::chrono::system_clock, R>>");
        }
    }
};

#endif//CONVERT_SQL_STD_CHRONO_TYPES_HH_147717D9BC013CC62D1C5A4692408CCF
