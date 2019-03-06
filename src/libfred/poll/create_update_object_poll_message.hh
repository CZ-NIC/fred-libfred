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
#ifndef CREATE_UPDATE_OBJECT_POLL_MESSAGE_HH_DAC36D20F83C4552BA16FD307CD80D1F
#define CREATE_UPDATE_OBJECT_POLL_MESSAGE_HH_DAC36D20F83C4552BA16FD307CD80D1F

#include "libfred/opcontext.hh"

/**
 *  @file
 *  create update object poll message
 */

namespace LibFred {
namespace Poll {

class CreateUpdateObjectPollMessage
{
public:
    /**
     * @param _ctx operation context
     * @param _history_id specific history version of object to which the new message shall be related
     * @throws LibFred::OperationException an operation specific exception
     * @throws LibFred::InternalError an unexpected exception
     */
    void exec(LibFred::OperationContext& _ctx, unsigned long long _history_id)const;
};

} // namespace LibFred::Poll
} // namespace LibFred

#endif
