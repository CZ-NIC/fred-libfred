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
#ifndef DATETIME_COMMON_HH_FE95828E5DF548469829B19110E3FD82
#define DATETIME_COMMON_HH_FE95828E5DF548469829B19110E3FD82

#include "util/types/common.hh"

namespace Database {

/*
 * date time element used for math operators
 */

#define DATETIME_ELEMENT(_name, _type) \
SIMPLE_WRAPPED_TYPE(_name, _type, 0)

DATETIME_ELEMENT(Days, unsigned)
DATETIME_ELEMENT(Months, unsigned)
DATETIME_ELEMENT(Years, unsigned)
DATETIME_ELEMENT(Hours, unsigned)
DATETIME_ELEMENT(Minutes, unsigned)
DATETIME_ELEMENT(Seconds, unsigned)


/*
 * special values definition
 */

/* special values for constructing date/time */
enum DateTimeSpecial {
  NOW,         //< actual local date
  NEG_INF,     //< minus infinity
  POS_INF,      //< plus infinity
  NOW_UTC       //< actual UTC date
};


/* special values for construction intervals */
enum DateTimeIntervalSpecial {
  NONE,        //< no special value
  DAY,         //< specific day
  INTERVAL,    //< specific interval
  LAST_HOUR,   //< constant for hour +- offset from/to actual time
  LAST_DAY,    //< constant for day +- offset from/to actual time
  LAST_WEEK,   //< constant for week +- offset from/to actual time
  LAST_MONTH,  //< constant for month +- offset from/to actual time
  LAST_YEAR,   //< constant for year +- offset from/to actual time
  PAST_HOUR,   //< constant for hour +- offset until/since actual time
  PAST_DAY,    //< constant for day +- offset until/since actual time
  PAST_WEEK,   //< constant for week +- offset until/since actual time
  PAST_MONTH,  //< constant for month +- offset until/since actual time
  PAST_YEAR    //< constant for year +- offset until/since actual time
};

}

#endif

