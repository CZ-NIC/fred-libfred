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

#ifndef GATHER_EMAIL_CONTENT_HH_3C4E3906E53448628AED7F0D087F0B8F
#define GATHER_EMAIL_CONTENT_HH_3C4E3906E53448628AED7F0D087F0B8F

#include "libfred/notifier/gather_email_data/notification_request.hh"
#include "libfred/opcontext.hh"

#include <map>
#include <string>

namespace Notification {

std::map<std::string, std::string> gather_email_content(
    const LibFred::OperationContext& _ctx,
    const notification_request& _request
);

}

#endif
