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
#include "util/types/date_interval.hh"

namespace Database {

DateInterval::DateInterval() : value(date(neg_infin), date(pos_infin)),
                               special(NONE),
      	                       offset(0) {
}


DateInterval::DateInterval(const Date& _start, const Date& _stop) : value(_start.value, _stop.value),
                                                                    special(INTERVAL),
                                                                    offset(0) {
}


DateInterval::DateInterval(DateTimeIntervalSpecial _s,
                           int _offset,
                           const Date& _start,
                           const Date& _stop) : value(_start.value, _stop.value),
                                                special(_s),
                                                offset(_offset) {

  if (special == DAY) {
    value = date_period(_start.get(), _start.get() + days(1));
  }
}


/* 
 * boost type manipulation compability 
 */ 

DateInterval::DateInterval(const date_period& _v) : value(_v),
                                      special(INTERVAL),
      				offset(0) {
}


const date_period& DateInterval::get() const {
  return value;
}


/*
 * string construct and getters 
 */

const std::string DateInterval::to_string() const {
  return str();
}


const std::string DateInterval::str() const {
  return to_simple_string(value);
}


/* 
 * interval bounds getters 
 */

const Date DateInterval::begin() const {
  return Date(value.begin());
}


const Date DateInterval::last() const {
  return Date(value.last());
}


const Date DateInterval::end() const {
  return Date(value.end());
}


/*
 * special value manipulation 
 */

bool DateInterval::isSpecial() const {
  return (special != NONE);
}


DateTimeIntervalSpecial DateInterval::getSpecial() const {
  return special;
}


int DateInterval::getSpecialOffset() const {
  return offset;
}


/*
 * date interval output operator 
 */

std::ostream& operator<<(std::ostream &_os, const DateInterval& _v) {
  return _os << _v.value;
}

}

