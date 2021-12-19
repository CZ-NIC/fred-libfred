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
#ifndef REGISTRAR_ZONE_ACCESS_HH_3F41B79AF3524A2598B8F2A2B2E1FC23
#define REGISTRAR_ZONE_ACCESS_HH_3F41B79AF3524A2598B8F2A2B2E1FC23

#include <boost/date_time/gregorian/gregorian.hpp>
#include "libfred/opcontext.hh"

namespace LibFred {

/**
 * check if the registrar has an access to the zone granted
 */
bool is_zone_accessible_by_registrar(
        unsigned long long _registrar_id,
        unsigned long long _zone_id,
        boost::gregorian::date _local_today,
        const OperationContext& _ctx);

} // namespace LibFred

#endif
