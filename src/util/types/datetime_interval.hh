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
#ifndef DATETIME_INTERVAL_HH_C3D122CA007649518D1C73DA29C3F9D6
#define DATETIME_INTERVAL_HH_C3D122CA007649518D1C73DA29C3F9D6

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

#include "util/types/datetime.hh"

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace Database {

/*
 * represents datetime interval type
 * - actually wrapped boost time_period type
 */


/* datetime interval implementation */
class DateTimeInterval {
public:
  /* constructors */
  DateTimeInterval();
  DateTimeInterval(const DateTime& _start, const DateTime& _stop);
  DateTimeInterval(DateTimeIntervalSpecial _s,
                   int _offset = 0,
                   const DateTime& _start = DateTime(NEG_INF),
             		   const DateTime& _stop  = DateTime(POS_INF)); 

  /* boost type manipulation compatibility */
  DateTimeInterval(const time_period& _v); 
  const time_period& get() const;

  /* string construct and getters */
  const std::string to_string() const;
  const std::string str() const;

  /* interval bounds getters */
  const DateTime begin() const; 
  const DateTime last() const;
  const DateTime end() const; 

  /* special value manipulation */
  bool isSpecial() const;
  DateTimeIntervalSpecial getSpecial() const; 
  int getSpecialOffset() const;

  /* datetime interval output operator */
  friend std::ostream& operator<<(std::ostream &_os, const DateTimeInterval& _v);

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
  time_period value;
  DateTimeIntervalSpecial special;
  int offset;
};

}

#endif /*DATETIME_INTERVAL_H_*/

