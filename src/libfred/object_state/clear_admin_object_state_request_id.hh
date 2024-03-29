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
 *  @file clear_admin_object_state_request_id.hh
 *  clear all administrative object state requests
 */

/*
administrativni zruseni vsech stavu blokovani objektu, (update do object_state_request) ClearAdminObjectStateRequestId
  M id objektu,
  M typ objektu,
  duvod
*/

#ifndef CLEAR_ADMIN_OBJECT_STATE_REQUEST_ID_HH_990A3C41B82647DE8BB643C27892AE53
#define CLEAR_ADMIN_OBJECT_STATE_REQUEST_ID_HH_990A3C41B82647DE8BB643C27892AE53

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"
#include "util/optional_value.hh"

#include <string>

namespace LibFred {

class ClearAdminObjectStateRequestId
{
public:
    ClearAdminObjectStateRequestId(ObjectId _object_id);
    ClearAdminObjectStateRequestId(ObjectId _object_id, const std::string &_reason);
    ClearAdminObjectStateRequestId& set_reason(const std::string &_reason);
    void exec(const OperationContext& _ctx);

    DECLARE_EXCEPTION_DATA(object_id_not_found, ObjectId);
    DECLARE_EXCEPTION_DATA(server_blocked_absent, ObjectId);

    struct Exception
        : virtual LibFred::OperationException,
          ExceptionData_object_id_not_found<Exception>,
          ExceptionData_server_blocked_absent<Exception>
    {};
private:
    const ObjectId object_id_;
    Optional<std::string> reason_;
};

}//namespace LibFred

#endif//CLEAR_ADMIN_OBJECT_STATE_REQUEST_ID_HH_990A3C41B82647DE8BB643C27892AE53
