/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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

#ifndef ADD_REGISTRAR_EPP_AUTH_HH_3704104C25B849B6B67818302E72B174
#define ADD_REGISTRAR_EPP_AUTH_HH_3704104C25B849B6B67818302E72B174

#include "libfred/opcontext.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include <string>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

class AddRegistrarEppAuth
{
public:
    explicit AddRegistrarEppAuth(
            std::string _registrar_handle,
            std::string _certificate_fingerprint,
            std::string _plain_password,
            std::string _cert_data_pem = no_cert_data);

    unsigned long long exec(const OperationContext& _ctx) const;

private:
    std::string registrar_handle_;
    std::string certificate_fingerprint_;
    std::string plain_password_;
    std::string cert_data_pem_;
};

EppAuthRecord add_registrar_epp_auth(
        const OperationContext& ctx,
        const std::string& registrar_handle,
        const std::string& certificate_fingerprint,
        const std::string& plain_password,
        const std::string& cert_data_pem = no_cert_data);

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
