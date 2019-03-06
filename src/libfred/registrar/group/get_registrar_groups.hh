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
 *  get registrar groups
 */

#ifndef GET_REGISTRAR_GROUPS_HH_46B31EA66921472A91C8206114D86775
#define GET_REGISTRAR_GROUPS_HH_46B31EA66921472A91C8206114D86775

#include "libfred/opcontext.hh"
#include "libfred/registrar/group/registrar_group_type.hh"

namespace LibFred {
namespace Registrar {

class GetRegistrarGroups
{
public:
    std::vector<RegistrarGroup> exec(OperationContext& _ctx);
};

} // namespace Registrar
} // namespace LibFred

#endif
