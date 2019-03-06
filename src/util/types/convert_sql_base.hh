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
/**
 *  @file convert_sql_base.h
 *  Definitions of type conversions to sql query string.
 */


#ifndef CONVERT_SQL_BASE_HH_345DF623CDB3426596BD9B5BF263FB84
#define CONVERT_SQL_BASE_HH_345DF623CDB3426596BD9B5BF263FB84


#include "util/types/convert_common.hh"


/**
 * no fallback convert if no specialization found
 * we want to know it for sql conversion
 */
template<class T>
struct SqlConvert {
  static T from(const std::string &_in);


  static std::string to(const T &_in);
};



template<>
struct SqlConvert<std::string> {
  static std::string from(const std::string &_in) {
    return _in;
  }


  static std::string to(const std::string &_in) {
    return _in;
  }
};


#endif /*CONVERT_SQL_BASE_H_*/

