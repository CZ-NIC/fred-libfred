/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
 *  info group membership by registrar
 */

#ifndef INFO_GROUP_MEMBERSHIP_BY_REGISTRAR_HH_58C70E419A5E4D1493A6BA59CF28C526
#define INFO_GROUP_MEMBERSHIP_BY_REGISTRAR_HH_58C70E419A5E4D1493A6BA59CF28C526

#include "libfred/registrar/group/membership/registrar_group_membership_types.hh"
#include "libfred/opcontext.hh"

#include <vector>

namespace LibFred {
namespace Registrar {

class InfoGroupMembershipByRegistrar
{
public:
    explicit InfoGroupMembershipByRegistrar(unsigned long long _registrar_id)
    : registrar_id_(_registrar_id)
    {}

    std::vector<GroupMembershipByRegistrar> exec(const OperationContext& _ctx);

private:
    unsigned long long registrar_id_;
};

} // namespace Registrar
} // namespace LibFred

#endif
