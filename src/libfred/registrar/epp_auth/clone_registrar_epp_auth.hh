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
#ifndef CLONE_REGISTRAR_EPP_AUTH_HH_7424349027DF40F9BD398B04054BC9CD
#define CLONE_REGISTRAR_EPP_AUTH_HH_7424349027DF40F9BD398B04054BC9CD

#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

class CloneRegistrarEppAuth
{
public:
    CloneRegistrarEppAuth(unsigned long long _id, const std::string& _certificate_fingerprint);

    unsigned long long exec(OperationContext& _ctx) const;

private:
    unsigned long long id_;
    std::string certificate_fingerprint_;
};

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
