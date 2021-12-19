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
#ifndef GET_REGISTRAR_ZONE_CREDIT_HH_D79316DD78B143EAB0BFE211A4C3AD0F
#define GET_REGISTRAR_ZONE_CREDIT_HH_D79316DD78B143EAB0BFE211A4C3AD0F

#include "libfred/registrar/registrar_zone_credit.hh"
#include "libfred/opcontext.hh"

namespace LibFred {

class GetRegistrarZoneCredit
{
public:
    RegistrarZoneCredit exec(const OperationContext& _ctx, const std::string& _registrar_handle)const;
};

} // namespace LibFred

#endif
