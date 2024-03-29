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
 *  @file copy_contact.hh
 *  copy contact
 */

#ifndef COPY_CONTACT_HH_B8E2D97E8D6D40A8A4E8A484379C2CC6
#define COPY_CONTACT_HH_B8E2D97E8D6D40A8A4E8A484379C2CC6

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"
#include "util/optional_value.hh"

#include <string>

namespace LibFred {

typedef unsigned long long RequestId;

class CopyContact
{
public:
    enum DbConst { OBJECT_TYPE_ID_CONTACT = 1 };//TODO: remove it!
    CopyContact(
            const std::string& _src_contact_handle,
            const std::string& _dst_contact_handle,
            RequestId _request_id);
    CopyContact(
            const std::string& _src_contact_handle,
            const std::string& _dst_contact_handle,
            const Optional<std::string>& _dst_registrar_handle,
            RequestId _request_id);
    CopyContact& set_registrar_handle(const std::string& _registrar_handle);
    ObjectId exec(const OperationContext& _ctx);

//exception impl
    DECLARE_EXCEPTION_DATA(src_contact_handle_not_found, std::string);
    DECLARE_EXCEPTION_DATA(dst_contact_handle_already_exist, std::string);
    DECLARE_EXCEPTION_DATA(create_contact_failed, std::string);

    struct Exception
        : virtual LibFred::OperationException,
          ExceptionData_src_contact_handle_not_found<Exception>,
          ExceptionData_dst_contact_handle_already_exist<Exception>,
          ExceptionData_create_contact_failed<Exception>
    { };
private:
    const std::string src_contact_handle_;
    const std::string dst_contact_handle_;
    Optional<std::string> dst_registrar_handle_;
    const RequestId request_id_;
};

}//namespace LibFred

#endif//COPY_CONTACT_HH_B8E2D97E8D6D40A8A4E8A484379C2CC6
