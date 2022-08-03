/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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

#ifndef DELETE_REGISTRAR_ZONE_ACCESS_HH_469FDAF3F07831D75F9FC6E8E6562A40//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define DELETE_REGISTRAR_ZONE_ACCESS_HH_469FDAF3F07831D75F9FC6E8E6562A40

#include "libfred/opcontext.hh"

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

class DeleteRegistrarZoneAccess
{
public:
    explicit DeleteRegistrarZoneAccess(unsigned long long access_id);
    unsigned long long exec(OperationContext& ctx) const;
private:
    unsigned long long access_id_;
};

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

#endif//DELETE_REGISTRAR_ZONE_ACCESS_HH_469FDAF3F07831D75F9FC6E8E6562A40
