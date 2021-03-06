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
 *  @file base_exception.h
 *  base class for any user defined exception.
 */


#ifndef BASE_EXCEPTION_HH_3DB5A362A6F1462DA3AB5E0B48E1A53A
#define BASE_EXCEPTION_HH_3DB5A362A6F1462DA3AB5E0B48E1A53A

#include <exception>
#include <string>

/**
 * \class Exception
 * \brief Base class for user defined group of exceptions
 */
class Exception : public std::exception
{
public:
    /**
     * Constructors and destructor
     */
	Exception(const Exception& _ex) noexcept
	    : std::exception(_ex),
	      what_(_ex.what_)
	{ }

    Exception(const std::string& _what) noexcept
        : what_(_what)
    { }

    Exception& operator=(const Exception& _ex) noexcept
	{
		what_ = _ex.what_;
		return *this;
	}

	~Exception() { }

  /**
   * @return  textual representation of exception
   */
	const char* what() const noexcept override
	{
		return what_.c_str();
	}
protected:
	std::string what_; /**< exception details in text form */
};

#endif /*BASE_EXCEPTION_H_*/
