/*
 * Copyright (C) 2012  CZ.NIC, z.s.p.o.
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
 *  @file
 *  Create request for administrative blocking of object.
 */

#ifndef CREATE_ADMIN_OBJECT_BLOCK_REQUEST_ID_HH_F574499E2EA143E4AF72C97243C9A7E1
#define CREATE_ADMIN_OBJECT_BLOCK_REQUEST_ID_HH_F574499E2EA143E4AF72C97243C9A7E1

#include "libfred/object_state/create_object_state_request_id.hh"
#include "libfred/object_state/typedefs.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"

#include <string>

namespace LibFred {

/**
* Create request for administrative blocking of object. Use integer id as domain identification.
* Created instance is modifiable by chainable methods i.e. methods returning instance reference.
* Data set into instance by constructor and methods serve as input data of the update.
* Creation is executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
* In case of wrong input data or other predictable and superable failure, an instance of @ref Exception is thrown
* with appropriate attributes set.
* In case of other insuperable failures and inconsistencies, an instance of @ref InternalError or other exception is thrown.
*/
class CreateAdminObjectBlockRequestId
{
public:
    typedef CreateObjectStateRequestId::Time Time; /**< common @ref LibFred::CreateObjectStateRequestId::Time "time" type */

    /**
    * Constructor with mandatory parameters.
    * @param _object_id sets domain id into @ref object_id_ attribute
    * @param _status_list sets states into @ref status_list_ attribute
    */
    CreateAdminObjectBlockRequestId(ObjectId _object_id, const StatusList& _status_list);

    /**
    * Constructor with all parameters.
    * @param _object_id sets domain id into @ref object_id_ attribute
    * @param _status_list sets states into @ref status_list_ attribute
    * @param _valid_from sets time into @ref valid_from_ attribute
    * @param _valid_to sets time into @ref valid_to_ attribute
    * @param _reason sets reason of blocking into @ref reason_ attribute
    * @param _logd_request_id sets sets logger request id into @ref logd_request_id_ attribute
    */
    CreateAdminObjectBlockRequestId(
            ObjectId _object_id,
            const StatusList& _status_list,
            const Optional<Time>& _valid_from,
            const Optional<Time>& _valid_to,
            const std::string& _reason,
            const Optional<unsigned long long>& _logd_request_id);

    /**
    * Sets time of validity starts
    * @param _valid_from sets time into @ref valid_from_ attribute
    * @return operation instance reference to allow method chaining
    */
    CreateAdminObjectBlockRequestId& set_valid_from(const Time& _valid_from);

    /**
    * Sets time of validity finishes
    * @param _valid_to sets time into @ref valid_to_ attribute
    * @return operation instance reference to allow method chaining
    */
    CreateAdminObjectBlockRequestId& set_valid_to(const Time& _valid_to);

    /**
    * Sets reason of blocking
    * @param _reason sets reason of blocking into @ref reason_ attribute
    * @return operation instance reference to allow method chaining
    */
    CreateAdminObjectBlockRequestId& set_reason(const std::string& _reason);

    /**
    * Sets logger request id
    * @param _logd_request_id sets logger request id into @ref logd_request_id_ attribute
    * @return operation instance reference to allow method chaining
    */
    CreateAdminObjectBlockRequestId& set_logd_request_id(unsigned long long _logd_request_id);

    /**
    * Executes creation
    * @param _ctx contains reference to database and logging interface
    * @return domain handle (fqdn)
    */
    std::string exec(OperationContext& _ctx);

    DECLARE_VECTOR_OF_EXCEPTION_DATA(state_not_found, std::string); /**< exception members for unknown state name generated by macro @ref DECLARE_VECTOR_OF_EXCEPTION_DATA*/
    DECLARE_EXCEPTION_DATA(server_blocked_present, ObjectId); /**< exception members in case domain is already blocked generated by macro @ref DECLARE_EXCEPTION_DATA*/

    /**
    * This exception is thrown in case of wrong input data or other predictable and superable failure.
    */
    struct Exception
        : virtual LibFred::OperationException,
          ExceptionData_vector_of_state_not_found<Exception>,
          ExceptionData_server_blocked_present<Exception>
    {};
private:
    void check_administrative_block_status_only(OperationContext& _ctx) const;
    void check_server_blocked_status_absent(OperationContext& _ctx) const;
    const ObjectId object_id_; /**< domain integer identificator */
    const StatusList status_list_; /**< list of status names to be set */
    Optional<Time> valid_from_; /**< status validity starts from */
    Optional<Time> valid_to_; /**< status is valid until this time */
    Optional<std::string> reason_; /**< reason of administrative blocking */
    Nullable<unsigned long long> logd_request_id_; /**< id of the record in logger database, id is used in other calls to logging within current request */
};

}//namespace LibFred

#endif//CREATE_ADMIN_OBJECT_BLOCK_REQUEST_ID_HH_F574499E2EA143E4AF72C97243C9A7E1
