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
#ifndef DATE_INTERVAL_HH_1BFEC07BC51D4D46A040B33189A3B02B
#define DATE_INTERVAL_HH_1BFEC07BC51D4D46A040B33189A3B02B

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/formatters.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/date_time/gregorian/greg_serialize.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#ifdef HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/string.hpp>
#endif

#include "util/types/date.hh"

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace Database {

class DateInterval {
public:
  DateInterval();
  DateInterval(const Date& _start, const Date& _stop); 
  DateInterval(DateTimeIntervalSpecial _s,
               int _offset = 0,
               const Date& _start = Date(NEG_INF),
               const Date& _stop  = Date(POS_INF));

  /* boost type manipulation compability */ 
  DateInterval(const date_period& _v);
  const date_period& get() const;

  /* string construct and getters */
  const std::string to_string() const;
  const std::string str() const; 

  /* interval bounds getters */
  const Date begin() const;
  const Date last() const; 
  const Date end() const;

  /* special value manipulation */
  bool isSpecial() const; 
  DateTimeIntervalSpecial getSpecial() const;
  int getSpecialOffset() const;

  /* date interval output operator */
  friend std::ostream& operator<<(std::ostream &_os, const DateInterval& _v);
  
#ifdef HAVE_BOOST_SERIALIZATION
  /* boost serialization */
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive& _ar,
      unsigned int) {
    _ar & BOOST_SERIALIZATION_NVP(value);
    _ar & BOOST_SERIALIZATION_NVP(special);
    _ar & BOOST_SERIALIZATION_NVP(offset);
  }
#endif

private:
  date_period             value;
  DateTimeIntervalSpecial special;
  int                     offset;
};

}

#endif /*DATE_INTERVAL*/

