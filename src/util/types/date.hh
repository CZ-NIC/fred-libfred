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
#ifndef DATE_HH_45CB1DDEE0064C1599EE72FD1A25E53E
#define DATE_HH_45CB1DDEE0064C1599EE72FD1A25E53E

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include "util/types/datetime_common.hh"
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
 * represents date type 
 * - actually wrapped boost library gregorian date type
 */

/* date class implementation */
class Date {
public:
  /* constructors */
  Date();
  Date(unsigned _y, unsigned _m, unsigned _d);
  Date(const std::string& _v); 
  Date(DateTimeSpecial _s);

  /* boost type manipulation compatibility */
  Date(const date& _v);
  const date& get() const;
  operator date() const {
    return value;
  }

  /* string construct and getters */
  void from_string(const std::string& _value);
  const std::string to_string() const;
  const std::string iso_str() const;
  const std::string str() const;
  bool is_special() const;
  int year() const;
  int month() const;
  int day() const;

  /* date comparison operators */
  friend bool operator<(const Date &_left, const Date &_right);
  friend bool operator>(const Date &_left, const Date &_right);
  friend bool operator<=(const Date &_left, const Date &_right);
  friend bool operator>=(const Date &_left, const Date &_right);
  friend bool operator==(const Date &_left, const Date &_right);
  friend bool operator!=(const Date &_left, const Date &_right);

  /* date math operator */
  friend Date operator+(const Date& _d, Days _days);
  friend Date operator+(const Date& _d, Months _months);
  friend Date operator+(const Date& _d, Years _years);
  friend Date operator-(const Date& _d, Days _days);
  friend Date operator-(const Date& _d, Months _months);
  friend Date operator-(const Date& _d, Years _years);

  /* date output operator */
  friend std::ostream& operator<<(std::ostream &_os, const Date& _v);


#ifdef HAVE_BOOST_SERIALIZATION
  /* boost serialization */
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive& _ar,
      unsigned int) {
    _ar & BOOST_SERIALIZATION_NVP(value);
  }
#endif

  friend class DateInterval;

private:
  date value;
};

}

#endif /*DATE_H_*/

