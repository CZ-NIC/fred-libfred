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
 *  declaration of PublicRequestObjectLockGuardByObjectId class
 */

#ifndef PUBLIC_REQUEST_OBJECT_LOCK_GUARD_HH_2EECCD07CAB44E26B9772ADE386A302C
#define PUBLIC_REQUEST_OBJECT_LOCK_GUARD_HH_2EECCD07CAB44E26B9772ADE386A302C

#include "libfred/object_state/typedefs.hh"
#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"

namespace LibFred {

/**
 * Common class guaranteeing exclusive read access to the public requests of given object.
 */
class LockedPublicRequestsOfObject
{
public:
    /**
     * Returns unique numeric id of object which public requests are locked.
     * @return object id
     */
    virtual ObjectId get_id()const = 0;
protected:
    virtual ~LockedPublicRequestsOfObject() { }
};

/**
 * Common class guaranteeing exclusive r/w access to the public requests of given object.
 */
class LockedPublicRequestsOfObjectForUpdate:public LockedPublicRequestsOfObject
{
public:
    /**
     * Returns operation context which has been used for the public requests locking.
     * @return reference to the operation context
     */
    virtual const OperationContext& get_ctx()const = 0;
protected:
    virtual ~LockedPublicRequestsOfObjectForUpdate() { }
};

/**
 * Obtain exclusive access to all public requests of given object.
 * @warning Destructor doesn't release lock contrary to expectations. The one will release by finishing of
 *          transaction wherein it was created.
 */
class PublicRequestsOfObjectLockGuardByObjectId:public LockedPublicRequestsOfObjectForUpdate
{
public:
    DECLARE_EXCEPTION_DATA(object_doesnt_exist, ObjectId);///< exception members for bad object_id
    struct Exception /// Something wrong happened
    :   virtual LibFred::OperationException,
        ExceptionData_object_doesnt_exist< Exception >
    {};
    /**
     * Obtain exclusive access to all public requests on object identified by _object_id. Operation context
     * _ctx can manipulate public request data from now until this transaction will finish.
     * @param _ctx use database connection from this operation context
     * @param _object_id unique numeric identification of object
     */
    PublicRequestsOfObjectLockGuardByObjectId(const OperationContext& _ctx, ObjectId _object_id);
    /**
     * @warning It doesn't release lock contrary to expectations. The one will release by finishing of
     *          transaction wherein it was created.
     */
    virtual ~PublicRequestsOfObjectLockGuardByObjectId() { }
private:
    virtual ObjectId get_id()const { return object_id_; }
    virtual const OperationContext& get_ctx()const { return ctx_; }
    const OperationContext& ctx_;
    const ObjectId object_id_;
};

} // namespace LibFred

#endif
