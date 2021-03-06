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
 *  @file
 *  print description of object data to the string and ostream for debugging and error handling
 */

#ifndef PRINTABLE_HH_5C88F872476A4AF5BC114777C1ECFB24
#define PRINTABLE_HH_5C88F872476A4AF5BC114777C1ECFB24

#include "util/util.hh"

#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <iterator>
#include <memory>

namespace Util {

/**
 * Base class that adds ostream& @ref operator<<.
 */
template <class D>
class Printable
{
private:
    /**
    * Dumps state of the instance into stream
    * @param os contains output stream reference
    * @param i reference of instance to be dumped into the stream
    * @return output stream reference
    */
    friend std::ostream& operator<<(std::ostream& os, const Printable& printable)
    {
        return os << static_cast<const D&>(printable).to_string();
    }
};

/**
 * Print state of operation into the string.
 * @param operation_name is name of the operation
 * @param key_value_list names and values of operation data members
 * @return description of operation state using implemented format
 */
std::string format_operation_state(const std::string& operation_name,
    const std::vector<std::pair<std::string, std::string>>& key_value_list);

/**
 * Print data of data-structure into the string.
 * @param data_structure_name is name of the data-structure
 * @param key_value_list names and values of the structure data members
 * @return description of the data-structure state using implemented format
 */
std::string format_data_structure(const std::string& data_structure_name,
    const std::vector<std::pair<std::string, std::string>>& key_value_list);

/**
 * Types of conversions to std::string detectable by @ref ConversionToString template
 */
struct TypeOfConversionToString
{
    enum Type
    {
        NONE,
        METHOD_TO_STRING,
        CONVERTIBLE_TO_STRING
    };
};

/**
 * Template detecting types of conversions to std::string
 * Using SFINAE. Might be later replaced by the Boost Type Traits Introspection library or C++ 11 type_traits.
 * @param T examined type
 * @return result member set to detected type of conversion viz @ref TypeOfCoversionToString
 */
template <typename T>
class ConversionToString
{
private:
    //return types have to differ in size
    struct NoConversionDetected { char setting_different_type_size[1]; };
    struct Method_to_string_Detected { char setting_different_type_size[2]; };
    struct ImplicitConversionDetected { char setting_different_type_size[3]; };

    //detection of method std::string T::to_string() const
    template <typename U, std::string (U::*)()const>
    struct MemberReturning_string_SignatureSpecificationUsingMemberPointer {};
    template <typename U>
    static Method_to_string_Detected detect_to_string_method(
            MemberReturning_string_SignatureSpecificationUsingMemberPointer<U, &U::to_string>*);
    template <typename U>
    static NoConversionDetected detect_to_string_method(...);

    //detection of T conversion to std::string
    static T makeT();//T constructor might not be accessible, so using this factory declaration instead
    static ImplicitConversionDetected detect_conversion_to_string(const std::string&);
    static NoConversionDetected detect_conversion_to_string(...);
public:
    /**
     * template parameter type
     */
    typedef T value_type;

    /**
     * Detected type of conversion
     */
    static const TypeOfConversionToString::Type result =
            (sizeof(detect_to_string_method<T>(0)) == sizeof(Method_to_string_Detected))
                ? TypeOfConversionToString::METHOD_TO_STRING
                : (sizeof(detect_conversion_to_string(makeT())) == sizeof(ImplicitConversionDetected))
                      ? TypeOfConversionToString::CONVERTIBLE_TO_STRING
                      : TypeOfConversionToString::NONE;
};

/**
 * Overload of conversion to std::string using to_string() method
 */
template <class T>
std::string printable_conversion_to_string(
        const T& t,
        EnumType<TypeOfConversionToString::METHOD_TO_STRING>)
{
    return t.to_string();
}

/**
 * Overload of conversion to std::string using implicit conversion
 */
template <class T>
std::string printable_conversion_to_string(
        const T& t,
        EnumType<TypeOfConversionToString::CONVERTIBLE_TO_STRING>)
{
    return t;
}

/**
 * Print container of elements into the string. Values from input container have to have conversion to string.
 * C++11 traits solution is a bit nicer viz http://stackoverflow.com/questions/7728478/c-template-class-function-with-arbitrary-container-type-how-to-define-it
 * @param in is input container with value_type member
 * @return string with content of input container delimited by separator
 */
template <class CONTAINER>
std::string format_container(const CONTAINER& in, std::string separator = " ")
{
    std::string out;
    typename CONTAINER::const_iterator i = in.begin();

    if (i != in.end())
    {
        out += printable_conversion_to_string(*i, EnumType<ConversionToString<typename CONTAINER::value_type>::result >());
        ++i;
    }

    for (; i != in.end(); ++i)
    {
        out += separator;
        out += printable_conversion_to_string(*i, EnumType<ConversionToString<typename CONTAINER::value_type>::result >());
    }
    return out;
}

/**
 * utility to check csv parser output
 */
template <class T>
std::string format_csv_data(const std::vector<std::vector<T>>& data)
{
    std::ostringstream ret;
    for (unsigned long long i = 0; i < data.size(); ++i)
    {
        ret << "#" << i << " |" << Util::format_container(data.at(i), "|") << "|";
    }
    return ret.str();
}

}//namespace Util

#endif//PRINTABLE_HH_5C88F872476A4AF5BC114777C1ECFB24
