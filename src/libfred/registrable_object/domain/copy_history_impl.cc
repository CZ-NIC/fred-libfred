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
#include "libfred/registrable_object/domain/copy_history_impl.hh"

namespace LibFred
{
    void copy_domain_data_to_domain_history_impl(
        const LibFred::OperationContext& _ctx,
        const unsigned long long _domain_id,
        const unsigned long long _historyid
    ) {
        const Database::Result res = _ctx.get_conn().exec_params(
            "INSERT INTO domain_history( "
                "historyid,  id, zone, registrant, nsset, exdate, keyset "
            ") "
            "SELECT "
                "$1::bigint, id, zone, registrant, nsset, exdate, keyset "
            "FROM domain "
            "WHERE id = $2::integer ",
            Database::query_param_list(_historyid)(_domain_id)
        );

        if (res.rows_affected() != 1) {
            throw std::runtime_error("INSERT INTO domain_history failed");
        }

        _ctx.get_conn().exec_params(
            "INSERT INTO domain_contact_map_history( "
                "historyid,  domainid, contactid, role "
            ") "
            "SELECT "
                "$1::bigint, domainid, contactid, role "
            "FROM domain_contact_map "
            "WHERE domainid = $2::integer ",
            Database::query_param_list(_historyid)(_domain_id)
        );

        _ctx.get_conn().exec_params(
            "INSERT INTO enumval_history( "
                "historyid,  domainid, exdate, publish "
            ") "
            "SELECT "
                "$1::bigint, domainid, exdate, publish "
            "FROM enumval "
            "WHERE domainid = $2::integer ",
            Database::query_param_list(_historyid)(_domain_id)
        );
    }
}
