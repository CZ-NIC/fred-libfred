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
 *  @file get_object_state_id_map.cc
 *  get object state id map
 */

#include "libfred/object_state/get_object_state_id_map.hh"
#include "libfred/opcontext.hh"

#include <sstream>

namespace LibFred {

GetObjectStateIdMap::GetObjectStateIdMap(const StatusList& _status_list, ObjectType _object_type)
    : status_list_(_status_list),
      object_type_(_object_type)
{}

GetObjectStateIdMap::StateIdMap& GetObjectStateIdMap::exec(const OperationContext& _ctx)
{
    GetObjectStateIdMap::get_result(_ctx, status_list_, object_type_, state_id_map_);
    if (state_id_map_.size() < status_list_.size())
    {
        std::string not_found;
        for (StatusList::const_iterator pState = status_list_.begin(); pState != status_list_.end(); ++pState)
        {
            if (state_id_map_.count(*pState) == 0)
            {
                not_found += " " + *pState;
            }
        }
        if (!not_found.empty())
        {
            BOOST_THROW_EXCEPTION(Exception().set_state_not_found(not_found));
        }
    }
    return state_id_map_;
}

GetObjectStateIdMap::StateIdMap& GetObjectStateIdMap::get_result(
        const OperationContext& _ctx,
        const StatusList& _status_list,
        ObjectType _object_type,
        StateIdMap& _result)
{
    _result.clear();
    if (_status_list.empty())
    {
        return _result;
    }
    StatusList::const_iterator pState = _status_list.begin();
    Database::query_param_list param(_object_type);
    param(*pState);
    ++pState;
    std::ostringstream query;
    query << "SELECT id,name "
             "FROM enum_object_states "
             "WHERE $1::INTEGER=ANY(types) AND "
                   "name IN ($" << param.size() << "::TEXT";
    while (pState != _status_list.end())
    {
        param(*pState);
        query << ",$" << param.size() << "::TEXT";
        ++pState;
    }
    query << ")";
    const Database::Result id_name_result = _ctx.get_conn().exec_params(query.str(), param);
    for (::size_t rowIdx = 0; rowIdx < id_name_result.size(); ++rowIdx)
    {
        const Database::Row& row = id_name_result[rowIdx];
        _result[row[1]] = row[0];
    }
    return _result;
}

}//namespace LibFred
