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
 *  nsset info implementation
 */

#ifndef INFO_NSSET_IMPL_HH_3F9A4813905741329F222603AC6A8143
#define INFO_NSSET_IMPL_HH_3F9A4813905741329F222603AC6A8143

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"
#include "util/printable.hh"
#include "util/db/param_query_composition.hh"
#include "libfred/registrable_object/nsset/info_nsset_output.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>
#include <utility>

namespace LibFred {

/**
 * Nsset info implementation.
 * It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
 */
class InfoNsset
{
public:
    /**
    * Default constructor.
    * Sets @ref history_query_ and @ref lock_ to false
    */
    InfoNsset();

    /**
     * Nsset info query projection aliases.
     * Set of constants for building inline view filter expressions.
     */
    struct GetAlias
    {
        static const char* id() { return "info_nsset_id"; }
        static const char* uuid() { return "info_nsset_uuid"; }
        static const char* roid() { return "info_nsset_roid"; }
        static const char* handle() { return "info_nsset_handle"; }
        static const char* delete_time() { return "info_nsset_delete_time"; }
        static const char* historyid() { return "info_nsset_historyid"; }
        static const char* history_uuid() { return "info_nsset_history_uuid"; }
        static const char* next_historyid() { return "info_nsset_next_historyid"; }
        static const char* history_valid_from() { return "info_nsset_history_valid_from"; }
        static const char* history_valid_to() { return "info_nsset_history_valid_to"; }
        static const char* sponsoring_registrar_id() { return "info_nsset_sponsoring_registrar_id"; }
        static const char* sponsoring_registrar_handle() { return "info_nsset_sponsoring_registrar_handle"; }
        static const char* creating_registrar_id() { return "info_nsset_creating_registrar_id"; }
        static const char* creating_registrar_handle() { return "info_nsset_creating_registrar_handle"; }
        static const char* last_updated_by_registrar_id() { return "info_nsset_last_updated_by_registrar_id"; }
        static const char* last_updated_by_registrar_handle() { return "info_nsset_last_updated_by_registrar_handle"; }
        static const char* creation_time() { return "info_nsset_creation_time"; }
        static const char* transfer_time() { return "info_nsset_transfer_time"; }
        static const char* update_time() { return "info_nsset_update_time"; }
        static const char* tech_check_level() { return "info_nsset_tech_check_level"; }
        static const char* authinfopw() { return "info_nsset_authinfopw"; }
        static const char* first_historyid() { return "info_nsset_first_historyid"; }
        static const char* logd_request_id() { return "info_nsset_logd_request_id"; }
        static const char* utc_timestamp() { return "info_nsset_utc_timestamp"; }
        static const char* local_timestamp() { return "info_nsset_local_timestamp"; }
    };

    /**
     * Sets nsset selection criteria.
     * Filter expression, which is optional WHERE clause, has access to @ref GetAlias info nsset projection aliases.
     * Simple usage example: .set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::id())(" = ").param_bigint(id_))
     */
    InfoNsset& set_inline_view_filter(const Database::ParamQuery& filter_expr);

    /**
     * Sets CTE query, that returns set of nsset id.
     */
    InfoNsset& set_cte_id_filter(const Database::ParamQuery& filter_expr);

    /**
    * Sets history query flag.
    * @param history_query sets history query flag into @ref history query_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNsset& set_history_query(bool history_query);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNsset& set_lock();

    /**
    * Executes getting info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the nsset descendingly ordered by nsset historyid
    */
    std::vector<InfoNssetOutput> exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "UTC")const;
private:
    Database::ParamQuery make_info_nsset_projection_query(const std::string& local_timestamp_pg_time_zone_name)const;
    Database::ParamQuery make_tech_contact_query(unsigned long long id, unsigned long long historyid)const;
    Database::ParamQuery make_dns_host_query(unsigned long long nssetid, unsigned long long historyid)const;
    Database::ParamQuery make_dns_ip_query(unsigned long long hostid, unsigned long long historyid)const;
    bool history_query_;/**< flag to query history records of the nsset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
    Optional<Database::ParamQuery> info_nsset_inline_view_filter_expr_;/**< where clause of the info nsset query where projection is inline view sub-select */
    Optional<Database::ParamQuery> info_nsset_id_filter_cte_;/**< CTE query returning set of nsset id */
};

}//namespace LibFred

#endif//INFO_NSSET_IMPL_HH_3F9A4813905741329F222603AC6A8143
