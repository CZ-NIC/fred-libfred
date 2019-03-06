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
 *  get object state descriptions
 */

#include "libfred/object_state/get_object_state_descriptions.hh"

namespace LibFred {

GetObjectStateDescriptions::GetObjectStateDescriptions(const std::string& description_language)
    : description_language_(description_language),
      external_states(false)
{}

GetObjectStateDescriptions& GetObjectStateDescriptions::set_external()
{
    external_states = true;
    return *this;
}

GetObjectStateDescriptions& GetObjectStateDescriptions::set_object_type(const std::string& object_type)
{
    object_type_ = object_type;
    return *this;
}

std::vector<ObjectStateDescription> GetObjectStateDescriptions::exec(OperationContext& ctx)
{
    Database::ParamQuery query(
        "SELECT eosd.state_id,"
               "eos.name,"
               "eosd.description "
        "FROM enum_object_states_desc eosd "
        "JOIN enum_object_states eos ON eos.id=eosd.state_id");

    if (external_states)
    {
        query(" AND eos.external");
    }

    if (!object_type_.empty())
    {
        query(" AND (SELECT id FROM enum_object_type WHERE name=").param_text(object_type_)(")=ANY(eos.types)");
    }

    query(" WHERE UPPER(eosd.lang)=UPPER(").param_text(description_language_)(")");

    const Database::Result object_state_descriptions_result = ctx.get_conn().exec_params(query);

    std::vector<ObjectStateDescription> result;
    for (unsigned long long idx = 0 ; idx < object_state_descriptions_result.size() ; ++idx)
    {
        result.push_back(ObjectStateDescription(
            static_cast<unsigned long long>(object_state_descriptions_result[idx][0]),
            static_cast<std::string>(object_state_descriptions_result[idx][1]),
            static_cast<std::string>(object_state_descriptions_result[idx][2])));
    }
    return result;
}

}//namespace LibFred
