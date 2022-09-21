/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#ifndef INFO_KEYSET_IMPL_HH_3E20CD14F99F472787BF0F1BADD6DE2F
#define INFO_KEYSET_IMPL_HH_3E20CD14F99F472787BF0F1BADD6DE2F

#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"
#include "util/db/param_query_composition.hh"
#include "libfred/registrable_object/keyset/info_keyset_output.hh"
#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
 * Keyset info implementation.
 * It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
 */
class InfoKeyset
{
public:
    /**
     * Keyset info query projection aliases.
     * Set of constants for building inline view filter expressions.
     */
    struct GetAlias
    {
        static const char* id() { return "info_keyset_id"; }
        static const char* uuid() { return "info_keyset_uuid"; }
        static const char* roid() { return "info_keyset_roid"; }
        static const char* handle() { return "info_keyset_handle"; }
        static const char* delete_time() { return "info_keyset_delete_time"; }
        static const char* historyid() { return "info_keyset_historyid"; }
        static const char* history_uuid() { return "info_keyset_history_uuid"; }
        static const char* next_historyid() { return "info_keyset_next_historyid"; }
        static const char* history_valid_from() { return "info_keyset_history_valid_from"; }
        static const char* history_valid_to() { return "info_keyset_history_valid_to"; }
        static const char* sponsoring_registrar_id() { return "info_keyset_sponsoring_registrar_id"; }
        static const char* sponsoring_registrar_handle() { return "info_keyset_sponsoring_registrar_handle"; }
        static const char* creating_registrar_id() { return "info_keyset_creating_registrar_id"; }
        static const char* creating_registrar_handle() { return "info_keyset_creating_registrar_handle"; }
        static const char* last_updated_by_registrar_id() { return "info_keyset_last_updated_by_registrar_id"; }
        static const char* last_updated_by_registrar_handle() { return "info_keyset_last_updated_by_registrar_handle"; }
        static const char* creation_time() { return "info_keyset_creation_time"; }
        static const char* transfer_time() { return "info_keyset_transfer_time"; }
        static const char* update_time() { return "info_keyset_update_time"; }
        static const char* first_historyid() { return "info_keyset_first_historyid"; }
        static const char* logd_request_id() { return "info_keyset_logd_request_id"; }
        static const char* utc_timestamp() { return "info_keyset_utc_timestamp"; }
        static const char* local_timestamp() { return "info_keyset_local_timestamp"; }
    };

    /**
    * Default constructor.
    * Sets @ref history_query_ and @ref lock_ to false
    */
    InfoKeyset();

    /**
     * Sets keyset selection criteria.
     * Filter expression, which is optional WHERE clause, has access to @ref GetAlias info keyset projection aliases.
     * Simple usage example: .set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::id())(" = ").param_bigint(id_))
     */
    InfoKeyset& set_inline_view_filter(const Database::ParamQuery& filter_expr);

    /**
     * Sets CTE query, that returns set of keyset id.
     */
    InfoKeyset& set_cte_id_filter(const Database::ParamQuery& filter_expr);

    /**
    * Sets history query flag.
    * @param history_query sets history query flag into @ref history query_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoKeyset& set_history_query(bool history_query);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeyset& set_lock();

    /**
    * Executes getting info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the keyset descendingly ordered by keyset historyid
    */
    std::vector<InfoKeysetOutput> exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "UTC")const;
private:
    Database::ParamQuery make_info_keyset_projection_query(const std::string& local_timestamp_pg_time_zone_name)const;
    Database::ParamQuery make_tech_contact_query(unsigned long long id, unsigned long long historyid)const;
    Database::ParamQuery make_dns_keys_query(unsigned long long id, unsigned long long historyid)const;
    bool history_query_;/**< flag to query history records of the keyset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
    Optional<Database::ParamQuery> info_keyset_inline_view_filter_expr_;/**< where clause of the info keyset query where projection is inline view sub-select */
    Optional<Database::ParamQuery> info_keyset_id_filter_cte_;/**< CTE query returning set of keyset id */
};

}//namespace LibFred

#endif
