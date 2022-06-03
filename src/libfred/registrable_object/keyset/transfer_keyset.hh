/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#ifndef TRANSFER_KEYSET_HH_5965E173BAB34AA287A3199A5E83D014
#define TRANSFER_KEYSET_HH_5965E173BAB34AA287A3199A5E83D014

#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"

#include <string>

namespace LibFred {

class TransferKeyset
{
public:
    explicit TransferKeyset(
            unsigned long long _keyset_id,
            std::string _new_registrar_handle,
            std::string _authinfopw,
            const Nullable<unsigned long long>& _logd_request_id);

    /**
     * @returns historyid of transferred keyset
     * @throws UnknownKeysetId
     * @throws UnknownRegistrar
     * @throws IncorrectAuthInfoPw
     * @throws NewRegistrarIsAlreadySponsoring
     */
    unsigned long long exec(OperationContext& _ctx) const;
private:
    unsigned long long keyset_id_;
    std::string new_registrar_handle_;
    std::string authinfopw_;
    Nullable<unsigned long long> logd_request_id_;
};

}//namespace LibFred

#endif//TRANSFER_KEYSET_HH_5965E173BAB34AA287A3199A5E83D014
