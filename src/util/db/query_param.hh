/*
 * Copyright (C) 2010  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file query_param.h
 *  Implementation of param object.
 */

#ifndef QUERY_PARAM_HH_FD0ED5831EF844BA88032F67BC9E2554
#define QUERY_PARAM_HH_FD0ED5831EF844BA88032F67BC9E2554

#include "util/db/nullable.hh"
#include "util/uuid.hh"

#include "libfred/registrable_object/uuid.hh"

#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <vector>
#include <string>
#include <ios>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace Database {

//buffer type
typedef std::string QueryParamData;


/**
 * \class  QueryParam
 * \brief  query parameter text , binary or null container
 */

class QueryParam
{
public:
    QueryParam(const QueryParam& param)
    : binary_(param.binary_)
    , null_(param.null_)
    , buffer_(param.buffer_)
    {}

    QueryParam& operator=(const QueryParam& param)
    {
        if (this != &param)
        {
            binary_=param.binary_;
            null_=param.null_;
            buffer_=param.buffer_;
        }
        return *this;
    }//operator=

    QueryParam()
    : binary_(false)
    , null_ (true)
    , buffer_("")
    {}

    QueryParam(const bool binary, const QueryParamData& data)
    : binary_(binary)
    , null_(false)
    , buffer_(data)
    {}

    QueryParam( const std::vector<unsigned char>& blob)
    : binary_(true)
    , null_(false)
    {
        buffer_.resize(blob.size());
        memcpy(const_cast<char *>(buffer_.data()), &(*blob.begin()), blob.size());
    }

    //posix time
    QueryParam(const boost::posix_time::ptime& value)
    : binary_(false)
    , null_(false)
    {
        buffer_ = boost::posix_time::to_iso_extended_string(value);
        size_t idx = buffer_.find('T');
        if (idx != std::string::npos) {
            buffer_[idx] = ' ';
        }
    }

    //gregorian date
    QueryParam(const boost::gregorian::date& value)
    : binary_(false)
    , null_(false)
    , buffer_( boost::gregorian::to_iso_extended_string(value))
    {}

    //uuid
    QueryParam(const uuid& value)
    : binary_(false)
    , null_(false)
    , buffer_(value)
    {}

    template <typename T, typename N>
    QueryParam(const Util::StrongType<T, N>& value)
        : binary_(false),
          null_(false),
          buffer_(Util::strong_to_string(value))
    { }

    template <class T>
    QueryParam(const Nullable<T>& t)
    : binary_(false)
    , null_(t.isnull())
    {
        if (!t.isnull())
        {
            buffer_ = boost::lexical_cast<std::string>(t.get_value());
        }
    }

    //all others
    template <class T>
    QueryParam(T t)
    : binary_(false)
    , null_(false)
    {

        //TODO: check valid combinations of basic param types with database types in query
        buffer_ = boost::lexical_cast<std::string>(t);
    }

    std::string print_buffer()const
    {
        if (null_)
        {
            return "null";
        }
        if (!binary_)
        {
            //std::cout << "Text param: " <<  buffer_<< std::endl;
            return buffer_;
        }
        if (buffer_.empty())
        {
            return "not set";
        }
        std::ostringstream hexdump;
        //std::cout << "Binary param: ";
        for (QueryParamData::const_iterator i = buffer_.begin(); i != buffer_.end(); ++i)
        {
            hexdump <<  std::setw(2) << std::setfill('0') << std::hex << std::uppercase
                    << static_cast<unsigned short>(static_cast<unsigned char>(*i));
            //std::cout << " " << hexdump.str();
        }
        //std::cout << std::endl;
        return hexdump.str();
    }

    std::string to_string() const
    {
        return print_buffer();
    }

    //getters
    const QueryParamData& get_data() const
    {
        return buffer_;
    }

    bool is_binary() const
    {
        return binary_;
    }

    bool is_null() const
    {
        return null_;
    }
private:
    //text or binary data flag
    bool binary_;
    //isnull flag
    bool null_;

    //text or binary
    QueryParamData buffer_;
};

//vector od query params initializable by query_param_list
typedef std::vector<QueryParam> QueryParams;

//template for initialization of container with push_back and value_type
template <typename CONTAINER_TYPE > struct list_of_params
    : public CONTAINER_TYPE
{
    typedef typename CONTAINER_TYPE::value_type ELEMENT_TYPE;
    list_of_params()
    { }

    list_of_params(const ELEMENT_TYPE& t)
    {
        (*this)(t);
    }
    list_of_params& operator()(const ELEMENT_TYPE& t)
    {
        this->push_back(t);
        return *this;
    }

    /***
     * @returns size of container AFTER new element has been pushed back
     * (i. e. number for parameter in sql query)
     */
    std::string add(const ELEMENT_TYPE& t) {
        this->push_back(t);
        return boost::lexical_cast<std::string>( CONTAINER_TYPE::size() );
    }
};

//boost assign list_of like initialization of params
typedef list_of_params<QueryParams> query_param_list;


const QueryParam QPNull;
const QueryParam NullQueryParam;

}//namespace Database

#endif//QUERY_PARAM_HH_FD0ED5831EF844BA88032F67BC9E2554
