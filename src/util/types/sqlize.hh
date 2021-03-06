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
 *  @file sqlize.h
 *  Interface for convert structures for convert from/to sql
 */

#ifndef SQLIZE_HH_8FDA493CEC1340129F6783C6BD6BC496
#define SQLIZE_HH_8FDA493CEC1340129F6783C6BD6BC496

#include "util/types/convert_sql_base.hh"


/**
 * converts value to format for sql query
 */
template<class T>
inline std::string sqlize(const T &_value) {
  return SqlConvert<T>::to(_value);
}



/**
 * converts value from database stored format to value type
 */
template<class T>
inline T unsqlize(const std::string &_value) {
  return SqlConvert<T>::from(_value);
}


#endif /*STRINGIFY_H_*/

