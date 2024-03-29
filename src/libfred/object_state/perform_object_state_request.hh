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
 *  perform object state request
 */

#ifndef PERFORM_OBJECT_STATE_REQUEST_HH_2CFAD473CF7E4138AE10AF167D322FB1
#define PERFORM_OBJECT_STATE_REQUEST_HH_2CFAD473CF7E4138AE10AF167D322FB1

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"

namespace LibFred
{
    /**
    * Process object state change requests.
    * Calling database function update_object_states with optional object_id.
    * It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
    * In case of insuperable failures and inconsistencies, exception is thrown.
    */
    class PerformObjectStateRequest
    {
    public:

        /**
        * Default behavior when no object_id is set (or is set to 0) is process all object state change requests.
        */
        PerformObjectStateRequest();

        /**
         * Process only object state change requests for object with given database id.
         * @param _object_id sets database id of the object @ref object_id_ attribute
         */
        PerformObjectStateRequest(const Optional< unsigned long long > &_object_id);

        /**
         * Sets database id of object for object state change requests processing.
         * @param _object_id sets database id of the object @ref object_id_ attribute
         * @return operation instance reference to allow method chaining
         */
        PerformObjectStateRequest& set_object_id(unsigned long long _object_id);

        /**
        * Executes object state change requests processing.
        * @param ctx contains reference to database and logging interface
        */
        void exec(const OperationContext& _ctx);

    private:
        Optional< unsigned long long > object_id_;/**< database id of the object */
    };//class PerformObjectStateRequest

} // namespace LibFred

#endif
