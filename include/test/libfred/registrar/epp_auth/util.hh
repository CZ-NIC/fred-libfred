/*
 * Copyright (C) 2020-2021  CZ.NIC, z. s. p. o.
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

#ifndef UTIL_HH_D98CD47F7B6A47699889AD58CD5A631A
#define UTIL_HH_D98CD47F7B6A47699889AD58CD5A631A

#include "libfred/opcontext.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include <boost/optional.hpp>

#include <string>

namespace Test {

unsigned long long get_epp_auth_id(
        const ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const boost::optional<std::string>& _plain_password = boost::none);

unsigned long long get_epp_auth_id(
        const ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password);

unsigned long long add_epp_authentications(
        const ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password);

::LibFred::Registrar::EppAuth::EppAuthRecord add_epp_authentications(
        const ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password,
        const std::string& _certificate_data_pem);

std::string get_random_fingerprint();

} //namespace Test

#endif
