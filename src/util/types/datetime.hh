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
#ifndef DATETIME_HH_BBEDC537E27C4BABB5DEE0CF7E69FDB2
#define DATETIME_HH_BBEDC537E27C4BABB5DEE0CF7E69FDB2

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include "util/types/datetime_common.hh"
#include "util/types/date.hh"
#include "config.h"

#ifdef HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/string.hpp>
#endif

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace Database {

/*
 * represent datetime type
 * - actually wrapped boost library ptime datetime type
 */

/* date time implementation */
class DateTime {
public:
  /* constructors */
  DateTime();
  DateTime(const Date& _v); 
  DateTime(DateTimeSpecial _s);
  DateTime(const std::string& _v);

  /* boost type manipulation compatibility */
  DateTime(const ptime& _v);
  const ptime& get() const;
  operator ptime() const {
    return value;
  }

  /* string construct and getters */ 
  void from_string(const std::string& _v);
  const std::string to_string() const;
  const std::string iso_str() const;
  const std::string str() const; 
  Date date() const;

  bool is_special() const;

  /* datetime comparison operators */
  friend bool operator<(const DateTime &_left, const DateTime &_right);
  friend bool operator>(const DateTime &_left, const DateTime &_right);
  friend bool operator<=(const DateTime &_left, const DateTime &_right);
  friend bool operator>=(const DateTime &_left, const DateTime &_right);
  friend bool operator==(const DateTime &_left, const DateTime &_right);
  friend bool operator!=(const DateTime &_left, const DateTime &_right);

  /* datetime math operators */
  friend DateTime operator+(const DateTime& _d, const Days& _days);
  friend DateTime operator+(const DateTime& _d, const Months& _months);
  friend DateTime operator+(const DateTime& _d, const Years& _years);
  friend DateTime operator+(const DateTime& _d, const Hours& _hours);
  friend DateTime operator+(const DateTime& _d, const Minutes& _minutes);
  friend DateTime operator+(const DateTime& _d, const Seconds& _seconds);
  friend DateTime operator-(const DateTime& _d, const Days& _days);
  friend DateTime operator-(const DateTime& _d, const Months& _months);
  friend DateTime operator-(const DateTime& _d, const Years& _years);
  friend DateTime operator-(const DateTime& _d, const Hours& _hours);
  friend DateTime operator-(const DateTime& _d, const Minutes& _minutes);
  friend DateTime operator-(const DateTime& _d, const Seconds& _seconds);

  /* datetime ouptut operator */
  friend std::ostream& operator<<(std::ostream &_os, const DateTime& _v);

  DateTime operator=(const DateTime &sec);

#ifdef HAVE_BOOST_SERIALIZATION
  /* boost serialization */
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive& _ar,
      unsigned int) {
    _ar & BOOST_SERIALIZATION_NVP(value);
  }
#endif

  friend class DateTimeInterval;

private:
  ptime value;
};

}

#endif /*DATETIME_H_*/

