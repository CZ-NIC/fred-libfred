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

#ifndef PROCESS_ONE_NOTIFICATION_REQUEST_HH_EACAB5C8B7DD482DBC796F7E6D5D0D4D
#define PROCESS_ONE_NOTIFICATION_REQUEST_HH_EACAB5C8B7DD482DBC796F7E6D5D0D4D

#include "libfred/mailer.hh"
#include "libfred/opcontext.hh"

#include "libfred/notifier/gather_email_data/notification_request.hh"

#include <map>
#include <set>
#include <string>
#include <memory>

namespace Notification {

struct FailedToSendMail {
    const notification_request failed_request_data;
    const std::string failed_recipient;
    const std::set<std::string> skipped_recipients;
    const std::string template_name;
    const std::map<std::string, std::string> template_parameters;

    FailedToSendMail(
        const notification_request& _failed_request_data,
        const std::string& _failed_recipient,
        const std::set<std::string>& _skipped_recipients,
        const std::string& _template_name,
        const std::map<std::string, std::string>& _template_parameters
    ) :
        failed_request_data(_failed_request_data),
        failed_recipient(_failed_recipient),
        skipped_recipients(_skipped_recipients),
        template_name(_template_name),
        template_parameters(_template_parameters)
    { }
};

struct FailedToLockRequest { };

/**
 * Process one notification enqueued in table notification_queue.
 * Respects do-not-send-empty-update-notification rule from ticket #6547 - skips notification of update where no change in data is found.
 * @returns true if any notification was processed (because of do-not-send-empty-update-notification rule it does not guarantee any e-mail was actually sent)
 * @throws FailedToSendMail
 * @throws FailedToLockRequest in case other transaction is holding lock
 */
bool process_one_notification_request(const LibFred::OperationContext& _ctx, std::shared_ptr<LibFred::Mailer::Manager> _mailer);

}
#endif
