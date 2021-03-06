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
#ifndef UTIL_HH_F54151D7C0B848C8926F03720CB06166
#define UTIL_HH_F54151D7C0B848C8926F03720CB06166

#include "util/types/data_types.hh"

#include <boost/format.hpp>

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <stdexcept>

namespace Util {

//TODO: move into tests
template <class CONTAINER>
typename CONTAINER::value_type get_nth(const CONTAINER& in, typename CONTAINER::size_type n)
{
    if (in.size() <= n)
    {
        throw std::out_of_range((boost::format("Util::get_nth n: %1%  size: %2%") % n % in.size()).str());
    }
    typename CONTAINER::const_iterator i = in.begin();
    std::advance(i, n);
    return *i;
}

//for given instance first call of get() (or first call of get() after reset() ) returns head , all other calls returns separator
//
class HeadSeparator
{
public:
    HeadSeparator(const std::string& head, const std::string& separator);
    std::string get();
    void reset();
private:
    bool got_head_;
    std::string head_;
    std::string separator_;
};


//template for initialization of vector
template <typename ELEMENT_TYPE >
struct vector_of : public std::vector<ELEMENT_TYPE>
{
    //appends one element
    vector_of(const ELEMENT_TYPE& t)
    {
        (*this)(t);
    }
    vector_of& operator()(const ELEMENT_TYPE& t)
    {
        this->push_back(t);
        return *this;
    }
    //appends vector of the same elements
    vector_of(const std::vector<ELEMENT_TYPE>& v)
    {
        (*this)(v);
    }
    vector_of& operator()(const std::vector<ELEMENT_TYPE>& v)
    {
        this->insert(this->end(), v.begin(), v.end());
        return *this;
    }
};

//template for initialization of std::set
template <typename ELEMENT_TYPE >
struct set_of : public std::set<ELEMENT_TYPE>
{
    //insert one element
    set_of(const ELEMENT_TYPE& t)
    {
        (*this)(t);
    }
    set_of& operator()(const ELEMENT_TYPE& t)
    {
        std::pair<typename std::set<ELEMENT_TYPE>::iterator, bool> insert_result = this->insert(t);
        if (!insert_result.second) throw std::logic_error("not unique");
        return *this;
    }
    //inserts set of the same elements
    set_of(const std::set<ELEMENT_TYPE>& v)
    {
        (*this)(v);
    }
    set_of& operator()(const std::set<ELEMENT_TYPE>& v)
    {
        this->insert(v.begin(), v.end());
        return *this;
    }
};

template <class T>
std::string container2comma_list(const T &_cont)
{
    if (_cont.empty())
    {
        return "";
    }

    std::ostringstream tmp;
    typename T::const_iterator it = _cont.begin();
    tmp << *it;
    for (++it; it != _cont.end(); ++it)
    {
        tmp << ", " << *it;
    }
    return tmp.str();
}

std::string escape(
        std::string input,
        const std::string& what = "'\\",
        const std::string& esc_by = "\\");

std::string escape2(std::string);

std::string make_svtrid(unsigned long long request_id);

/**
 * Makes type from enum value
 * @param VALUE is value of the enum
 */
template <int VALUE>
struct EnumType
{
   enum { value = VALUE };
};

}//namespace Util

#endif//UTIL_HH_F54151D7C0B848C8926F03720CB06166
