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
 */

#ifndef TRANSFER_NSSET_HH_9FEE728C0EE448DCB9F7D79194625F0E
#define TRANSFER_NSSET_HH_9FEE728C0EE448DCB9F7D79194625F0E

#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"

#include <string>

namespace LibFred {

class TransferNsset
{
public:
    TransferNsset(
        const unsigned long long _nsset_id,
        const std::string& _new_registrar_handle,
        const std::string& _authinfopw_for_authorization,
        const Nullable<unsigned long long>& _logd_request_id
    );

    /**
     * @throws UnknownNssetId
     * @throws UnknownRegistrar
     * @throws IncorrectAuthInfoPw
     * @throws NewRegistrarIsAlreadySponsoring
     */
    unsigned long long exec(OperationContext& _ctx);

private:
    const unsigned long long nsset_id_;
    const std::string new_registrar_handle_;
    const std::string authinfopw_for_authorization_;
    const Nullable<unsigned long long> logd_request_id_;
};

}//namespace LibFred

#endif//TRANSFER_NSSET_HH_9FEE728C0EE448DCB9F7D79194625F0E
