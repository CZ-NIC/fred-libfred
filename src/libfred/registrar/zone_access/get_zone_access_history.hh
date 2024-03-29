/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#ifndef GET_ZONE_ACCESS_HISTORY_HH_D244602CE4E742B08CA40AE73028E1EF
#define GET_ZONE_ACCESS_HISTORY_HH_D244602CE4E742B08CA40AE73028E1EF

#include "libfred/opcontext.hh"
#include "libfred/registrar/zone_access/registrar_zone_access_history.hh"

#include <exception>
#include <string>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

class GetZoneAccessHistory
{
public:
    explicit GetZoneAccessHistory(const std::string& _registrar_handle);
    RegistrarZoneAccessHistory exec(const OperationContext& _ctx) const;
    struct Exception : virtual std::exception { };
private:
    std::string registrar_handle_;
};

}//namespace LibFred::Registrar::ZoneAccess
}//namespace LibFred::Registrar
}//namespace LibFred

#endif//GET_ZONE_ACCESS_HISTORY_HH_D244602CE4E742B08CA40AE73028E1EF
