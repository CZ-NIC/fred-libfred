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
 */

#ifndef COPY_HISTORY_IMPL_HH_D092044C2ADC4A5CBAFE8A624358AEAF
#define COPY_HISTORY_IMPL_HH_D092044C2ADC4A5CBAFE8A624358AEAF

#include "libfred/opcontext.hh"


namespace LibFred
{
    /**
     * Copy record from table 'nsset' identified by _nsset_id to table 'nsset_history'. New record will use _historyid.
     * Copy record from table 'host' identified by _nsset_id to table 'host_history'. New record will use _historyid.
     * Copy record from table 'host_ipaddr_map' identified by _nsset_id to table 'host_ipaddr_map_history'. New record will use _historyid.
     * Copy record from table 'nsset_contact_map' identified by _nsset_id to table 'nsset_contact_map_history'. New record will use _historyid.
     * @param _historyid MUST be existing id in table 'history'
     */
    void copy_nsset_data_to_nsset_history_impl(
        const LibFred::OperationContext& _ctx,
        unsigned long long _nsset_id,
        unsigned long long _historyid
    );
}

#endif
