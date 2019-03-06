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
#ifndef ID_HH_D595630589994F598A0C6260B6A5BFAE
#define ID_HH_D595630589994F598A0C6260B6A5BFAE

#include <ostream>
#include <istream>
#include "config.h"

#ifdef HAVE_BOOST_SERIALIZATION
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#endif

namespace Database {

/*
 * class represents id type
 */

class ID {
public:
  typedef unsigned long long value_type;
  /* constructors */
  ID() : value_(0) {
  }

  ID(value_type _value) : value_(_value) {
  }


  /* comparison operators */
//  friend bool operator<(const ID& _left, const ID& _right);
//  friend bool operator>(const ID& _left, const ID& _right);
//  friend bool operator<=(const ID& _left, const ID& _right);
//  friend bool operator>=(const ID& _left, const ID& _right);
//  friend bool operator==(const ID& _left, const ID& _right);
//  friend bool operator!=(const ID& _left, const ID& _right);

  /* output operator */
  friend std::ostream& operator<<(std::ostream &_os, const ID& _v);
  friend std::istream& operator>>(std::istream &_is, ID& _v);


  operator value_type() const {
    return value_;
  }

#ifdef HAVE_BOOST_SERIALIZATION
  /* boost serialization */
  friend class boost::serialization::access;
  template<class Archive> void serialize(Archive& _ar,
      unsigned int) {
    _ar & BOOST_SERIALIZATION_NVP(value_);
  }
#endif

private:
  value_type value_;
};

}

#endif /*ID_H_*/

