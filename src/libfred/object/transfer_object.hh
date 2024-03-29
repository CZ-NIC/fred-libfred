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
#ifndef TRANSFER_OBJECT_HH_3F72B450B4BA47DB8160FC4563A0E562
#define TRANSFER_OBJECT_HH_3F72B450B4BA47DB8160FC4563A0E562

#include "libfred/opcontext.hh"

#include <set>
#include <string>

namespace LibFred {

/**
 * @returns historyid of transferred object
 * @throws UnknownRegistrar
 * @throws UnknownObjectId
 * @throws IncorrectAuthInfoPw
 * @throws NewRegistrarIsAlreadySponsoring
 */
unsigned long long transfer_object(
        const LibFred::OperationContext& _ctx,
        unsigned long long _object_id,
        const std::string& _new_registrar_handle,
        const std::string& _authinfopw,
        const std::set<std::string>& _enabled_contacts,
        const Nullable<unsigned long long>& _logd_request_id = Nullable<unsigned long long>());

}//namespace LibFred

#endif
