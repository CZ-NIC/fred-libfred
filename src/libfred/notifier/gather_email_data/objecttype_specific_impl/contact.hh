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
 *  contact specific notification implementation
 */

#ifndef CONTACT_HH_3BBBCE559F3D4FB59261C5C3AC35CDDC
#define CONTACT_HH_3BBBCE559F3D4FB59261C5C3AC35CDDC

#include "libfred/opcontext.hh"
#include "libfred/notifier/event_on_object_enum.hh"
#include "libfred/registrable_object/contact/info_contact_data.hh"

#include <string>
#include <map>

namespace Notification {

std::map<std::string, std::string> gather_contact_data_change(
        const LibFred::OperationContext& _ctx,
        notified_event _event,
        unsigned long long _history_id_post_change);

std::set<std::string> get_emails_to_notify_contact_event(
        const LibFred::OperationContext& _ctx,
        notified_event _event,
        unsigned long long _history_id_after_change);

std::string to_template_handle(LibFred::ContactAddressType::Value _type);

}//namespace Notification

#endif//CONTACT_HH_3BBBCE559F3D4FB59261C5C3AC35CDDC
