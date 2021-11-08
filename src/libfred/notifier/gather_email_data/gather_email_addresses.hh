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

#ifndef GATHER_EMAIL_ADDRESSES_HH_A7F1C3A8599647FEB69BC0931A7C3FC6
#define GATHER_EMAIL_ADDRESSES_HH_A7F1C3A8599647FEB69BC0931A7C3FC6

#include "libfred/opcontext.hh"
#include "libfred/notifier/event_on_object_enum.hh"

#include <set>
#include <string>

namespace Notification {

std::set<std::string> gather_email_addresses(
    const LibFred::OperationContext& _ctx,
    const EventOnObject& _event_on_object,
    unsigned long long _history_id_post_change
);

}

#endif
