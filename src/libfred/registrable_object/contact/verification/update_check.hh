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
 *  @file
 *  update contact check
 */

#ifndef UPDATE_CHECK_HH_E798B553EDBA4E33B125A5F90979F3B9
#define UPDATE_CHECK_HH_E798B553EDBA4E33B125A5F90979F3B9

#include "util/printable.hh"
#include "util/db/nullable.hh"
#include "util/optional_value.hh"
#include "util/uuid.hh"

#include "libfred/registrable_object/contact/verification/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {

/**
 * Updates existing record in contact_check table. Has no sideeffects.
 */
class UpdateContactCheck : public Util::Printable<UpdateContactCheck>
{
public:
    /**
     * constructor only with mandatory parameters
     * @param _check_handle     identifies which contact_check to update.
     * @param _status_handle    denotes status to be set. Allowed values are in enum_check_status.h or enum_contact_check_status in database.
     */
    UpdateContactCheck(const uuid& _check_handle,
                       const std::string& _status_handle);

    /**
     * constructor with all available parameters including optional ones
     * @param _check_handle     identifies which contact_check to update by it's handle.
     * @param _status_handle    denotes status to be set. Allowed values are in enum_check_status.h or enum_contact_check_status.name in database.
     * @param _logd_request_id  denotes entry in log_entry database table related to this update.
     */
    UpdateContactCheck(const uuid& _check_handle,
                       const std::string& _status_handle,
                       const Optional<unsigned long long>& _logd_request_id);

    /**
     * setter of optional logd_request_id
     * Call with another value for re-set, no need to unset first.
     */
    UpdateContactCheck& set_logd_request_id(unsigned long long _logd_request_id);

    /**
     * Commits operation.
     * @throws LibFred::ExceptionUnknownCheckHandle
     * @throws LibFred::ExceptionUnknownCheckStatusHandle
     */
    void exec(const OperationContext& _ctx);

    std::string to_string()const;
private:
    uuid check_handle_;
    std::string status_handle_;
    Nullable<unsigned long long> logd_request_id_;
};

}//namespace LibFred

#endif//UPDATE_CHECK_HH_E798B553EDBA4E33B125A5F90979F3B9
