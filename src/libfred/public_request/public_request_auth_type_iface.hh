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
 *  @file
 *  declaration of PublicRequestAuthTypeIface class
 */

#ifndef PUBLIC_REQUEST_AUTH_TYPE_IFACE_HH_CCE79FEBE5884A72A08D508B4AF24BC8
#define PUBLIC_REQUEST_AUTH_TYPE_IFACE_HH_CCE79FEBE5884A72A08D508B4AF24BC8

#include "libfred/public_request/public_request_type_iface.hh"
#include "libfred/public_request/public_request_object_lock_guard.hh"

namespace LibFred {

/**
 * Common class for type of particular public request with authentication.
 */
class PublicRequestAuthTypeIface:public PublicRequestTypeIface
{
public:
    /**
     * Generate unique password for new public request authentication.
     * @param _locked_contact contact joined with public request (password can be derived from contact's authinfopw)
     * @return password
     */
    virtual std::string generate_passwords(const LockedPublicRequestsOfObjectForUpdate &_locked_contact)const = 0;
    /**
     * Instance pointer is publicly deletable.
     */
    virtual ~PublicRequestAuthTypeIface() { }
};

} // namespace LibFred

#endif
