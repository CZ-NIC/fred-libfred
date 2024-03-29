/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
 *  @file clear_object_state_request_id.hh
 *  clear object state request
 */

#ifndef CLEAR_OBJECT_STATE_REQUEST_ID_HH_7FF13ECC79004EC8A6E4D18F5AED5FF9
#define CLEAR_OBJECT_STATE_REQUEST_ID_HH_7FF13ECC79004EC8A6E4D18F5AED5FF9

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace LibFred {

class ClearObjectStateRequestId
{
public:
    ClearObjectStateRequestId(ObjectId _object_id);
    typedef std::vector<ObjectId> Requests;
    Requests exec(const OperationContext& _ctx);

    DECLARE_EXCEPTION_DATA(object_id_not_found, ObjectId);

    struct Exception
    :   virtual LibFred::OperationException,
        ExceptionData_object_id_not_found<Exception>
    {};
private:
    const ObjectId object_id_;
};

}//namespace LibFred

#endif//CLEAR_OBJECT_STATE_REQUEST_ID_HH_7FF13ECC79004EC8A6E4D18F5AED5FF9
