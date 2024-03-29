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
 *  declaration of PublicRequestLockGuard class family
 */

#ifndef PUBLIC_REQUEST_LOCK_GUARD_HH_609F3162F0BA4F5DA24B1CA8A344A8D2
#define PUBLIC_REQUEST_LOCK_GUARD_HH_609F3162F0BA4F5DA24B1CA8A344A8D2

#include "libfred/object_state/typedefs.hh"
#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"

namespace LibFred {

/**
 * Common class guaranteeing exclusive read access to the data of given public request.
 */
class LockedPublicRequest
{
public:
    /**
     * Returns unique numeric id of locked public request.
     * @return id of locked public request
     */
    virtual PublicRequestId get_id()const = 0;
protected:
    virtual ~LockedPublicRequest() { }
};

/**
 * Common class guaranteeing exclusive r/w access to the data of given public request.
 */
class LockedPublicRequestForUpdate:public LockedPublicRequest
{
public:
    /**
     * Returns operation context which has been used for the public request locking.
     * @return reference to the operation context
     */
    virtual const OperationContext& get_ctx()const = 0;
protected:
    virtual ~LockedPublicRequestForUpdate() { }
};

/**
 * Obtain exclusive access to public request data identified by string identification.
 */
class PublicRequestLockGuardByIdentification:public LockedPublicRequestForUpdate
{
public:
    DECLARE_EXCEPTION_DATA(public_request_doesnt_exist, std::string);///< exception members for unknown identification
    struct Exception /// Something wrong happened
    :   virtual LibFred::OperationException,
        ExceptionData_public_request_doesnt_exist< Exception >
    {};
    /**
     * Obtain exclusive access to the public request identified by _identification. Operation context _ctx
     * can manipulate public request data from now until this transaction will finish.
     * @param _ctx use database connection from this operation context
     * @param _identification unique string identification of public request with authentication
     */
    PublicRequestLockGuardByIdentification(const OperationContext& _ctx, const std::string &_identification);
    /**
     * @warning It doesn't release lock contrary to expectations. The one will release by finishing of
     *          transaction wherein it was created.
     */
    virtual ~PublicRequestLockGuardByIdentification() { }
private:
    virtual PublicRequestId get_id()const { return public_request_id_; }
    virtual const OperationContext& get_ctx()const { return ctx_; }
    const OperationContext& ctx_;
    const PublicRequestId public_request_id_;
};

/**
 * Obtain exclusive access to public request data identified by numeric identification.
 */
class PublicRequestLockGuardById:public LockedPublicRequestForUpdate
{
public:
    DECLARE_EXCEPTION_DATA(public_request_doesnt_exist, PublicRequestId);///< exception members for unknown id
    struct Exception /// Something wrong happened
    :   virtual LibFred::OperationException,
        ExceptionData_public_request_doesnt_exist< Exception >
    {};
    /**
     * Obtain exclusive access to public request identified by _id. Operation context _ctx
     * can manipulate public request data from now until this transaction will finish.
     * @param _ctx use database connection from this operation context
     * @param _id unique numeric identification of public request
     */
    PublicRequestLockGuardById(const OperationContext& _ctx, PublicRequestId _id);
    /**
     * @warning It doesn't release lock contrary to expectations. The one will release by finishing of
     *          transaction wherein it was created.
     */
    virtual ~PublicRequestLockGuardById() { }
private:
    virtual PublicRequestId get_id()const { return public_request_id_; }
    virtual const OperationContext& get_ctx()const { return ctx_; }
    const OperationContext& ctx_;
    const PublicRequestId public_request_id_;
};

} // namespace LibFred

#endif
