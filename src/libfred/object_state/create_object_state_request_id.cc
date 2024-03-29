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
 *  @file create_object_state_request_id.cc
 *  create object state request
 */

#include "libfred/object_state/create_object_state_request_id.hh"
#include "libfred/object_state/get_blocking_status_desc_list.hh"
#include "libfred/object_state/get_object_state_id_map.hh"
#include "libfred/object_state/lock_object_state_request_lock.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"

#include <boost/algorithm/string.hpp>

#include <sstream>

namespace LibFred {

CreateObjectStateRequestId::CreateObjectStateRequestId(
        ObjectId _object_id,
        const StatusList &_status_list)
    : object_id_(_object_id),
      status_list_(_status_list)
{}

CreateObjectStateRequestId::CreateObjectStateRequestId(
        ObjectId _object_id,
        const StatusList& _status_list,
        const Optional<Time>& _valid_from,
        const Optional<Time>& _valid_to)
    : object_id_(_object_id),
      status_list_(_status_list),
      valid_from_(_valid_from),
      valid_to_(_valid_to)
{}

CreateObjectStateRequestId& CreateObjectStateRequestId::set_valid_from(const Time &_valid_from)
{
    valid_from_ = _valid_from;
    return *this;
}

CreateObjectStateRequestId& CreateObjectStateRequestId::set_valid_to(const Time &_valid_to)
{
    valid_to_ = _valid_to;
    return *this;
}

namespace {

void check_valid_status(const OperationContext& _ctx, ObjectId _object_id, const StatusList& _status_list);

}//namespace LibFred::{anonymous}

std::pair<std::string, unsigned long long> CreateObjectStateRequestId::exec(const OperationContext& _ctx)
{
    std::string object_state_names;

    for (StatusList::const_iterator pState = status_list_.begin();
         pState != status_list_.end(); ++pState)
    {
        object_state_names += (*pState) + " ";
    }

    FREDLOG_DEBUG(
        "CreateObjectStateRequestId::exec object id: " + boost::lexical_cast< std::string >(object_id_) +
        " object state name: " + object_state_names +
        " valid from: " + boost::posix_time::to_iso_string(valid_from_.get_value_or_default()) +
        " valid to: " + boost::posix_time::to_iso_string(valid_to_.get_value_or_default()));

    //check time
    if (valid_to_.isset())
    {
        if (valid_from_.isset()) // <from, to)
        {
            if (valid_to_.get_value() < valid_from_.get_value())
            {
                std::string errmsg("valid from-to <");
                errmsg += boost::posix_time::to_iso_string(valid_from_.get_value()) + ", " +
                          boost::posix_time::to_iso_string(valid_to_.get_value()) + ")";
                BOOST_THROW_EXCEPTION(Exception().set_out_of_turn(errmsg));
            }
        }
        else // <now, to)
        {
            const Database::Result out_of_turn_result = _ctx.get_conn().exec_params(
                    "SELECT $1<CURRENT_TIMESTAMP",
                    Database::query_param_list(valid_to_.get_value()));
            if (static_cast<bool>(out_of_turn_result[0][0]))
            {
                std::string errmsg("valid from-to <CURRENT_TIMESTAMP, ");
                errmsg += boost::posix_time::to_iso_string(valid_to_.get_value()) + ")";
                BOOST_THROW_EXCEPTION(Exception().set_out_of_turn(errmsg));
            }
        }
    }

    //get object type
    ObjectType object_type = 0;
    std::string handle_name;
    Database::query_param_list param(object_id_);
    {
        const Database::Result object_type_result = _ctx.get_conn().exec_params(
            "SELECT type,name "
            "FROM object_registry "
            "WHERE id=$1::bigint", param);
        if (object_type_result.size() <= 0)
        {
            BOOST_THROW_EXCEPTION(Exception().set_object_id_not_found(object_id_));
        }
        check_valid_status(_ctx, object_id_, status_list_);
        const Database::Row& row = object_type_result[0];
        object_type = static_cast<ObjectType>(row[0]);
        handle_name = static_cast<std::string>(row[1]);
    }

    GetObjectStateIdMap get_object_state_id_map(status_list_, object_type);
    typedef GetObjectStateIdMap::StateIdMap StateIdMap;
    const StateIdMap& state_id_map = get_object_state_id_map.exec(_ctx);

    LockObjectStateRequestLock(object_id_).exec(_ctx);

    std::string object_state_id_set;
    for (StateIdMap::const_iterator pStateId = state_id_map.begin();
         pStateId != state_id_map.end(); ++pStateId)
    {
        const ObjectStateId object_state_id = pStateId->second;
        if (object_state_id_set.empty())
        {
            object_state_id_set = "(";
        }
        else
        {
            object_state_id_set += ",";
        }
        object_state_id_set += boost::lexical_cast< std::string >(object_state_id);
    }
    object_state_id_set += ")";

    std::string new_valid_column;
    if (valid_from_.isset())
    {
        if (valid_to_.isset()) // <from, to)
        {
            new_valid_column = "$2::timestamp AS new_valid_from,$3::timestamp AS new_valid_to";
            param(valid_from_.get_value())(valid_to_.get_value());
        }
        else // <from, oo)
        {
            new_valid_column = "$2::timestamp AS new_valid_from,NULL::timestamp AS new_valid_to";
            param(valid_from_.get_value());
        }
    }
    else if (valid_to_.isset()) // <now, to)
    {
        new_valid_column = "CURRENT_TIMESTAMP::timestamp AS new_valid_from,$2::timestamp AS new_valid_to";
        param(valid_to_.get_value());
    }
    else // <now, oo)
    {
        new_valid_column = "CURRENT_TIMESTAMP::timestamp AS new_valid_from,NULL::timestamp AS new_valid_to";
    }
    const std::string sub_query =
            "SELECT valid_from AS obj_valid_from,"
                   "LEAST(canceled,valid_to) AS obj_valid_to," +
                   new_valid_column + " "
            "FROM object_state_request "
            "WHERE object_id=$1::bigint AND state_id IN " + object_state_id_set;
    const Database::Result invalid_state_result = _ctx.get_conn().exec_params(
            "SELECT obj_valid_from,obj_valid_to,new_valid_from,new_valid_to "
            "FROM (" + sub_query + ") AS obj_state "
            "WHERE obj_valid_to<obj_valid_from OR "
                  "(obj_valid_from<=new_valid_from AND (new_valid_from<obj_valid_to OR obj_valid_to IS NULL)) OR "
                  "((obj_valid_from<new_valid_to OR new_valid_to IS NULL) AND (new_valid_to<=obj_valid_to OR obj_valid_to IS NULL)) "
            "LIMIT 1", param);

    if (0 < invalid_state_result.size())
    {
        const Database::Row &row = invalid_state_result[0];
        const boost::posix_time::ptime obj_valid_from = static_cast<const boost::posix_time::ptime&>(row[0]);
        const boost::posix_time::ptime obj_valid_to = row[1].isnull()
                                                          ? boost::posix_time::ptime(boost::posix_time::pos_infin)
                                                          : static_cast<const boost::posix_time::ptime&>(row[1]);
        if (obj_valid_to < obj_valid_from )
        {
            std::string errmsg("valid from-to <");
            errmsg += boost::posix_time::to_iso_string(obj_valid_from) + ", " +
                      boost::posix_time::to_iso_string(obj_valid_to) + ")";
            BOOST_THROW_EXCEPTION(Exception().set_out_of_turn(errmsg));
        }
        const boost::posix_time::ptime new_valid_from = static_cast<const boost::posix_time::ptime&>(row[2]);
        const boost::posix_time::ptime new_valid_to = row[3].isnull()
                                                        ? boost::posix_time::ptime(boost::posix_time::pos_infin)
                                                        : static_cast<const boost::posix_time::ptime&>(row[3]);
        std::string errmsg("object:");
        errmsg += boost::lexical_cast< std::string >(object_id_) + " "
                  "<" + boost::posix_time::to_iso_string(obj_valid_from) + ", " +
                  boost::posix_time::to_iso_string(obj_valid_to) + ") - "
                  "<" + boost::posix_time::to_iso_string(new_valid_from) + ", " +
                  boost::posix_time::to_iso_string(new_valid_to) + ")";
        BOOST_THROW_EXCEPTION(Exception().set_overlayed_time_intervals(errmsg));
    }

    param.clear();
    param(object_id_) // $1
         (valid_to_.isset() // $2
                         ? Database::QueryParam(valid_to_.get_value())
                         : Database::QPNull);
    std::ostringstream cmd;
    cmd << "INSERT INTO object_state_request "
                   "(object_id,"
                    "state_id,"
                    "crdate,"
                    "valid_from,"
                    "valid_to) VALUES ";
    if (valid_from_.isset())
    {
        param(valid_from_.get_value()); // $3
        for (StateIdMap::const_iterator pStateId = state_id_map.begin();
             pStateId != state_id_map.end(); ++pStateId)
        {
            if (pStateId != state_id_map.begin())
            {
                cmd << ",";
            }
            param(pStateId->second);
            cmd << "($1::bigint,"
                    "$" << param.size() << "::bigint,"
                    "CURRENT_TIMESTAMP,"
                    "$3::timestamp,"
                    "$2::timestamp)";
        }
    }
    else
    {
        for (StateIdMap::const_iterator pStateId = state_id_map.begin();
             pStateId != state_id_map.end(); ++pStateId)
        {
            if (pStateId != state_id_map.begin())
            {
                cmd << ",";
            }
            param(pStateId->second);
            cmd << "($1::bigint," <<
                    "$" << param.size() << "::bigint,"
                    "CURRENT_TIMESTAMP,"
                    "CURRENT_TIMESTAMP,"
                    "$2::timestamp)";
        }
    }

    cmd << " RETURNING id AS id_";

    const Database::Result result_id = _ctx.get_conn().exec_params(cmd.str(), param);

    return std::make_pair(handle_name, static_cast<unsigned long long>(result_id[0][0]));
}

namespace {

void check_valid_status(const OperationContext& _ctx, ObjectId _object_id, const StatusList &_status_list)
{
    if (_status_list.empty())
    {
        return;
    }
    Database::query_param_list param(_object_id);
    std::ostringstream query;
    query << "SELECT eos.name "
             "FROM enum_object_states eos "
             "JOIN object_registry obr ON obr.type=ANY(types) "
             "WHERE obr.id=$1::bigint AND "
                   "eos.manual AND "
                   "eos.name IN ($2::text";
    StatusList::const_iterator pStatus = _status_list.begin();
    param(*pStatus);
    ++pStatus;
    while (pStatus != _status_list.end())
    {
        param(*pStatus);
        query << ",$" << param.size() << "::text";
        ++pStatus;
    }
    query << ")";
    const Database::Result state_result = _ctx.get_conn().exec_params(query.str(), param);
    if (state_result.size() == _status_list.size())
    {
        return;
    }
    CreateObjectStateRequestId::Exception e;
    StatusList correct_status_list;
    for (std::size_t idx = 0; idx < state_result.size(); ++idx)
    {
        correct_status_list.insert(static_cast<std::string>(state_result[idx][0]));
    }
    for (StatusList::const_iterator pStatus = _status_list.begin(); pStatus != _status_list.end(); ++pStatus)
    {
        if (correct_status_list.find(*pStatus) == correct_status_list.end())
        {
            e.add_state_not_found(*pStatus);
        }
    }
    BOOST_THROW_EXCEPTION(e);
}

}//namespace LibFred::{anonymous}

}//namespace LibFred
