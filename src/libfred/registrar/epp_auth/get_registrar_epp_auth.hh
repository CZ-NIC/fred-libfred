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

#ifndef GET_REGISTRAR_EPP_AUTH_HH_84F019F1ECE041A5BBC1196190DA150F
#define GET_REGISTRAR_EPP_AUTH_HH_84F019F1ECE041A5BBC1196190DA150F

#include "libfred/opcontext.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include <string>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

class GetRegistrarEppAuth
{
public:
    explicit GetRegistrarEppAuth(std::string _registrar_handle);

    RegistrarEppAuthData exec(const OperationContext& _ctx) const;

private:
    std::string registrar_handle_;
};

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
