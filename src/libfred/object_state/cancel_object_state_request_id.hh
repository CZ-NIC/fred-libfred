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
 *  @file cancel_object_state_request_id.hh
 *  cancel object state request
 */

#ifndef CANCEL_OBJECT_STATE_REQUEST_ID_HH_DC869EB936FA49D393D44E764C0110E4
#define CANCEL_OBJECT_STATE_REQUEST_ID_HH_DC869EB936FA49D393D44E764C0110E4

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace LibFred {

class CancelObjectStateRequestId
{
public:
    typedef boost::posix_time::ptime Time;
    CancelObjectStateRequestId(
            ObjectId _object_id,
            const StatusList &_status_list);
    void exec(const OperationContext& _ctx);

//exception impl
    DECLARE_EXCEPTION_DATA(object_id_not_found, ObjectId);
    DECLARE_EXCEPTION_DATA(state_not_found, std::string);

    struct Exception
    :   virtual LibFred::OperationException,
        ExceptionData_object_id_not_found<Exception>,
        ExceptionData_state_not_found<Exception>
    {};
private:
    const ObjectId object_id_;
    const StatusList status_list_; //list of status names to be canceled
};

}//namespace LibFred

#endif//CANCEL_OBJECT_STATE_REQUEST_ID_HH_DC869EB936FA49D393D44E764C0110E4
