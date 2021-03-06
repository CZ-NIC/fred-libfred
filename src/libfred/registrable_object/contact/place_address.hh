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
 * @file
 * Common type for postal addresses of contacts.
 */

#ifndef PLACE_ADDRESS_HH_2E5EE8A3E6DE409AB8A0D01800797A6E
#define PLACE_ADDRESS_HH_2E5EE8A3E6DE409AB8A0D01800797A6E

#include "util/optional_value.hh"

#include <iosfwd>
#include <string>

namespace LibFred {
namespace Contact {

/**
 * Postal address of place (building).
 *
 * Common type for postal addresses of contacts.
 */
struct PlaceAddress
{
    PlaceAddress();

    /**
     * Constructor with custom initialization of attributes
     */
    PlaceAddress(const std::string& _street1,
        const Optional<std::string>& _street2,
        const Optional<std::string>& _street3,
        const std::string& _city,
        const Optional<std::string>& _stateorprovince,
        const std::string& _postalcode,
        const std::string& _country);

    /**
     * Dumps content into the string.
     * @return string with description of the instance content
     */
    std::string to_string()const;
    /**
     * Check equality of two instances.
     * @param _b compares @a this instance with @a _b instance
     * @return true if they are the same.
     */
    bool operator==(const PlaceAddress& _b)const;

    std::string street1;/**< address part 1 (required) */
    Optional< std::string > street2;/**< address part 2 (optional) */
    Optional< std::string > street3;/**< address part 3 (optional) */
    std::string city;/**< city (required) */
    Optional< std::string > stateorprovince;/**< state or province (optional) */
    std::string postalcode;/**< postal code (required) */
    std::string country;/**< country (required) */
};

/**
 * Inserts string representation that conforms value of @a src into stream @a out.
 * @param out object where characters are inserted
 * @param src object with the content to insert
 * @return the same as parameter @a out
 */
std::ostream& operator<<(std::ostream& out, const PlaceAddress& src);

}//namespace LibFred::Contact
}//namespace LibFred

#endif//PLACE_ADDRESS_HH_2E5EE8A3E6DE409AB8A0D01800797A6E
