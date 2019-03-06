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
 *  @file get_blocking_status_desc_list.h
 *  get blocking status desc list
 */

#ifndef GET_BLOCKING_STATUS_DESC_LIST_HH_46493E5A90F8457598C9C7392F80A875
#define GET_BLOCKING_STATUS_DESC_LIST_HH_46493E5A90F8457598C9C7392F80A875

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"
#include "util/optional_value.hh"

#include <string>
#include <vector>

namespace LibFred {

class GetBlockingStatusDescList
{
public:
    GetBlockingStatusDescList();
    GetBlockingStatusDescList(const Optional<std::string>& _lang, const Optional<ObjectType>& _object_type);
    GetBlockingStatusDescList& set_lang(const std::string& _lang);//language EN/CS
    GetBlockingStatusDescList& set_object_type(ObjectType _object_type);

    struct StatusDesc
    {
        StatusDesc() : state_id(0) {}
        StatusDesc(unsigned long long _state_id, std::string _status, std::string _desc)
            : state_id(_state_id),
              status(_status),
              desc(_desc)
        {}
        StatusDesc(const StatusDesc& _src)
            : state_id(_src.state_id),
              status(_src.status),
              desc(_src.desc)
        {}
        unsigned long long state_id;
        std::string status;
        std::string desc;
    };
    typedef std::vector<StatusDesc> StatusDescList;

    StatusDescList& exec(OperationContext& _ctx);

    DECLARE_EXCEPTION_DATA(lang_not_found, std::string);

    struct Exception
        : virtual LibFred::OperationException,
          ExceptionData_lang_not_found<Exception>
    {};
private:
    Optional<std::string> lang_;
    Optional<ObjectType> object_type_;
    StatusDescList status_desc_list_;
};

}//namespace LibFred

#endif//GET_BLOCKING_STATUS_DESC_LIST_HH_46493E5A90F8457598C9C7392F80A875
