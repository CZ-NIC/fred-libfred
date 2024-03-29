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
#include "libfred/poll/create_update_object_poll_message.hh"
#include "libfred/poll/create_poll_message.hh"
#include "libfred/object/object_type.hh"
#include "libfred/opexception.hh"

namespace LibFred {
namespace Poll {

void CreateUpdateObjectPollMessage::exec(const LibFred::OperationContext& _ctx, unsigned long long _history_id)const
{
    const Database::Result db_res = _ctx.get_conn().exec_params(
            "SELECT eot.name "
            "FROM object_registry obr "
            "JOIN object_history oh ON oh.id=obr.id "
            "JOIN enum_object_type eot ON eot.id=obr.type "
            "WHERE oh.historyid=$1::BIGINT",
            Database::query_param_list(_history_id));

    switch (db_res.size())
    {
        case 0:
        {
            struct NotFound:OperationException
            {
                const char* what()const noexcept { return "object history not found"; }
            };
            throw NotFound();
        }
        case 1:
            break;
        default:
        {
            struct TooManyRows:InternalError
            {
                TooManyRows():InternalError("too many rows") { }
            };
            throw TooManyRows();
        }
    }
    switch (Conversion::Enums::from_db_handle<Object_Type>(static_cast<std::string>(db_res[0][0])))
    {
        case Object_Type::contact:
            CreateUpdateOperationPollMessage<Object_Type::contact>().exec(_ctx, _history_id);
            return;
        case Object_Type::domain:
            CreatePollMessage<MessageType::update_domain>().exec(_ctx, _history_id);
            return;
        case Object_Type::keyset:
            CreatePollMessage<MessageType::update_keyset>().exec(_ctx, _history_id);
            return;
        case Object_Type::nsset:
            CreatePollMessage<MessageType::update_nsset>().exec(_ctx, _history_id);
            return;
    }
    struct UnexpectedObjectType:InternalError
    {
        UnexpectedObjectType():InternalError("unexpected object type") { }
    };
    throw UnexpectedObjectType();
}


} // namespace LibFred::Poll
} // namespace LibFred
