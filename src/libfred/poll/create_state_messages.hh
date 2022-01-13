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
#ifndef CREATE_STATE_MESSAGES_HH_0A85298C178C4EAE98F36E3289C83390
#define CREATE_STATE_MESSAGES_HH_0A85298C178C4EAE98F36E3289C83390

#include "libfred/opcontext.hh"
#include "libfred/poll/message_type_set.hh"

#include <boost/optional.hpp>

#include <set>
#include <string>

namespace LibFred {
namespace Poll {

class CreateStateMessages
{
public:
    CreateStateMessages(const std::set<LibFred::Poll::MessageType::Enum>& _except_list, const boost::optional<int>& _limit);
    unsigned long long exec(const OperationContext& _ctx) const;
private:
    std::set<LibFred::Poll::MessageType::Enum> except_list_;
    boost::optional<int> limit_;
};

} // namespace LibFred::Poll
} // namespace LibFred

#endif
