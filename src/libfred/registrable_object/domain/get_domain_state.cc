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
#include "libfred/registrable_object/domain/get_domain_state.hh"
#include "libfred/registrable_object/state_flag_setter.hh"
#include "libfred/registrable_object/exceptions_impl.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

GetDomainStateById::GetDomainStateById(unsigned long long domain_id)
    : domain_id_(domain_id)
{
}

GetDomainStateById::Result GetDomainStateById::exec(const OperationContext& ctx) const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    const std::string sql =
            "WITH o AS ("
                "SELECT id,type "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=$" + params.add(domain_id_) + "::BIGINT) "
            "SELECT eos.name "
            "FROM o "
            "LEFT JOIN object_state os ON os.object_id=o.id AND "
                                         "os.valid_to IS NULL "
            "LEFT JOIN enum_object_states eos ON eos.id=os.state_id AND "
                                                "o.type=ANY(eos.types)";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        FREDLOG_DEBUG(Conversion::Enums::to_db_handle(object_type) + " does not exist");
        throw DoesNotExist();
    }
    Result state;
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][0].isnull();
    if (object_has_state_flags)
    {
        for (std::size_t idx = 0; idx < dbres.size(); ++idx)
        {
            const std::string flag_name = static_cast<std::string>(dbres[idx][0]);
            state.template visit<StateFlagSetter>(flag_name);
        }
    }
    return state;
}

GetDomainStateByFqdn::GetDomainStateByFqdn(const std::string& fqdn)
    : fqdn_(fqdn)
{
}

GetDomainStateByFqdn::Result GetDomainStateByFqdn::exec(const OperationContext& ctx) const
{
    static const std::string sql_handle_case_normalize_function =
            object_type == Object_Type::domain ? "LOWER"
                                               : "UPPER";
    Database::query_param_list params;
    const std::string sql =
            "WITH o AS ("
                "SELECT id,type "
                "FROM object_registry "
                "WHERE type=get_object_type_id($" + params.add(Conversion::Enums::to_db_handle(object_type)) + "::TEXT) AND "
                      "name=" + sql_handle_case_normalize_function + "($" + params.add(fqdn_) + "::TEXT) AND "
                      "erdate IS NULL) "
            "SELECT eos.name "
            "FROM o "
            "LEFT JOIN object_state os ON os.object_id=o.id AND "
                                         "os.valid_to IS NULL "
            "LEFT JOIN enum_object_states eos ON eos.id=os.state_id AND "
                                                "o.type=ANY(eos.types)";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        FREDLOG_DEBUG(Conversion::Enums::to_db_handle(object_type) + " does not exist");
        throw DoesNotExist();
    }
    Result state;
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][0].isnull();
    if (object_has_state_flags)
    {
        for (std::size_t idx = 0; idx < dbres.size(); ++idx)
        {
            const std::string flag_name = static_cast<std::string>(dbres[idx][0]);
            state.template visit<StateFlagSetter>(flag_name);
        }
    }
    return state;
}

GetDomainStateByUuid::GetDomainStateByUuid(const DomainUuid& domain_uuid)
    : uuid_(domain_uuid)
{
}

GetDomainStateByUuid::Result GetDomainStateByUuid::exec(const OperationContext& ctx) const
{
    Database::query_param_list params;
    const auto object_type_param_text = "$" + params.add(Conversion::Enums::to_db_handle(object_type)) + "::TEXT";
    const std::string sql =
            "WITH o AS ("
                "SELECT id,type "
                "FROM object_registry "
                "WHERE uuid=$" + params.add(uuid_) + "::UUID AND "
                      "type=get_object_type_id(" + object_type_param_text + ")) "
            "SELECT eos.name "
            "FROM o "
            "LEFT JOIN object_state os ON os.object_id=o.id AND "
                                         "os.valid_to IS NULL "
            "LEFT JOIN enum_object_states eos ON eos.id=os.state_id AND "
                                                "o.type=ANY(eos.types)";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        FREDLOG_DEBUG(Conversion::Enums::to_db_handle(object_type) + " does not exist");
        throw DoesNotExist();
    }
    Result state;
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][0].isnull();
    if (object_has_state_flags)
    {
        for (std::size_t idx = 0; idx < dbres.size(); ++idx)
        {
            const std::string flag_name = static_cast<std::string>(dbres[idx][0]);
            state.template visit<StateFlagSetter>(flag_name);
        }
    }
    return state;
}

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred
