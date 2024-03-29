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
 */

#ifndef ENQUEUE_NOTIFICATION_HH_6415858177B14D99A28F0F0437CDA49C
#define ENQUEUE_NOTIFICATION_HH_6415858177B14D99A28F0F0437CDA49C

#include "libfred/notifier/event_on_object_enum.hh"
#include "libfred/opcontext.hh"

#include <string>

namespace Notification {

/**
 * Stores request for notification of change identified by _event_on_object and _object_historyid_post_change.
 *
 * Due to model inconsistency _object_historyid_post_change for notification of object deletion is actually not "post change" but "pre change".
 * It represents last history version of object before delete. Reason is that delete is not represented by separate history version.
 *
 * @param _svtrid Is result of Util::make_svtrid() called with appropriate Logger request id.
 */
void enqueue_notification(
    const LibFred::OperationContext& _ctx,
    const notified_event    _event,
    unsigned long long      _done_by_registrar,
    unsigned long long      _object_historyid_post_change,
    const std::string&      _svtrid
);

}
#endif
