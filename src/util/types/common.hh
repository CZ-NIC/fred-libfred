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
#ifndef COMMON_HH_103FBF6903A44AF3B33EA27A2FEC15DB
#define COMMON_HH_103FBF6903A44AF3B33EA27A2FEC15DB

#include <ostream>

#define SIMPLE_WRAPPED_TYPE(_name, _wrapped_type, _init)                   \
class _name {                                                              \
private:                                                                   \
  _wrapped_type value_;                                                    \
                                                                           \
public:                                                                    \
  typedef _wrapped_type value_type;                                        \
                                                                           \
  _name() : value_(_init) {                                                \
  }                                                                        \
                                                                           \
  _name(const _wrapped_type& _v) : value_(_v) {                            \
  }                                                                        \
                                                                           \
  operator _wrapped_type() const {                                         \
    return value_;                                                         \
  }                                                                        \
                                                                           \
  friend std::ostream& operator<<(std::ostream& _os, const _name& _value); \
};                                                                         \
                                                                           \
inline std::ostream& operator<<(std::ostream& _os, const _name& _v) {      \
  return _os << _v.value_;                                                 \
}


#endif /*COMMON_H_*/
