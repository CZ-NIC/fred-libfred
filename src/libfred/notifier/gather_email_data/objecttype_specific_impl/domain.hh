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
 *  domain specific notification implementation
 */

#ifndef DOMAIN_HH_4FE595060EF44641B41C68646B7BD330
#define DOMAIN_HH_4FE595060EF44641B41C68646B7BD330

#include "libfred/opcontext.hh"
#include "libfred/notifier/event_on_object_enum.hh"

#include <string>
#include <map>

namespace Notification {

    std::map<std::string, std::string> gather_domain_data_change(
        const LibFred::OperationContext& _ctx,
        const notified_event& _event,
        unsigned long long _history_id_post_change
    );

    std::set<unsigned long long> gather_contact_ids_to_notify_domain_event(
        const LibFred::OperationContext& _ctx,
        notified_event _event,
        unsigned long long _history_id_after_change
    );
}

#endif
