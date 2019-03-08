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
#include "libfred/registrable_object/keyset/copy_history_impl.hh"

namespace LibFred
{
    void copy_keyset_data_to_keyset_history_impl(
        LibFred::OperationContext& _ctx,
        const unsigned long long _keyset_id,
        const unsigned long long _historyid
    ) {
        const Database::Result res = _ctx.get_conn().exec_params(
            "INSERT INTO keyset_history( "
                "historyid,  id"
            ") "
            "SELECT "
                "$1::bigint, id "
            "FROM keyset "
            "WHERE id = $2::integer ",
            Database::query_param_list(_historyid)(_keyset_id)
        );

        if (res.rows_affected() != 1) {
            throw std::runtime_error("INSERT INTO keyset_history failed");
        }

        _ctx.get_conn().exec_params(
            "INSERT INTO dsrecord_history( "
                "historyid,  id, keysetid, keytag, alg, digesttype, digest, maxsiglife "
            ") "
            "SELECT "
                "$1::bigint, id, keysetid, keytag, alg, digesttype, digest, maxsiglife "
            "FROM dsrecord "
            "WHERE keysetid = $2::integer ",
            Database::query_param_list(_historyid)(_keyset_id)
        );

        _ctx.get_conn().exec_params(
            "INSERT INTO dnskey_history( "
                "historyid,  id, keysetid, flags, protocol, alg, key "
            ") "
            "SELECT "
                "$1::bigint, id, keysetid, flags, protocol, alg, key "
            "FROM dnskey "
            "WHERE keysetid = $2::integer ",
            Database::query_param_list(_historyid)(_keyset_id)
        );

        _ctx.get_conn().exec_params(
            "INSERT INTO keyset_contact_map_history( "
                "historyid,  keysetid, contactid "
            ") "
            "SELECT "
                "$1::bigint, keysetid, contactid "
            "FROM keyset_contact_map "
            "WHERE keysetid = $2::integer ",
            Database::query_param_list(_historyid)(_keyset_id)
        );

    }
}
