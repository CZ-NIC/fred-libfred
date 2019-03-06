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
 */

#ifndef GET_PREVIOUS_OBJECT_HISTORYID_HH_D9A16262E4FC41E3A94AAF1838E777F8
#define GET_PREVIOUS_OBJECT_HISTORYID_HH_D9A16262E4FC41E3A94AAF1838E777F8

#include "libfred/notifier/exception.hh"

#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"

namespace LibFred {

    struct ExceptionMultipleObjectHistories {
        const char* what() const { return "multiple object histories"; }
    };

    /**
     * @throws ExceptionMultipleObjectHistories in case history table has multiple records with given history_id
     */
    inline Nullable<unsigned long long> get_previous_object_historyid(LibFred::OperationContext& _ctx, unsigned long long history_id) {

        const Database::Result older_history_id_res = _ctx.get_conn().exec_params(
            "SELECT id FROM history WHERE next = $1::integer", Database::query_param_list(history_id)
        );

        if (older_history_id_res.size() > 1) {
            throw ExceptionMultipleObjectHistories();
        }

        if (older_history_id_res.size() < 1) {
            return Nullable<unsigned long long>();
        }

        return static_cast<unsigned long long>( older_history_id_res[0]["id"] );
    }

}

#endif
