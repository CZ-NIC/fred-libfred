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
#ifndef CREATE_POLL_MESSAGE_HH_EB853FB4A0A44A39A8A8E8EBC25BB3CC
#define CREATE_POLL_MESSAGE_HH_EB853FB4A0A44A39A8A8E8EBC25BB3CC

#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/poll/message_type.hh"

#include <set>
/**
 *  @file
 *  common implementation for creating epp action poll messages
 */

namespace LibFred {
namespace Poll {

/**
 * @tparam message_type create message of given type
 */
template <MessageType::Enum message_type>
struct CreatePollMessage
{
    /**
     * @param _ctx operation context
     * @param _history_id specific history version of registry object to which the new message shall be related
     * @return id of newly created message
     * @throws LibFred::OperationException an operation specific exception
     * @throws LibFred::InternalError an unexpected exception
     */
    unsigned long long exec(const LibFred::OperationContext& _ctx, unsigned long long _history_id) const;
};


template<Object_Type::Enum object_type>
struct CreateUpdateOperationPollMessage
{
    using Result = std::set<unsigned long long>;
    Result exec(const LibFred::OperationContext& _ctx, unsigned long long _history_id) const;
};

} // namespace LibFred::Poll
} // namespace LibFred

#endif
