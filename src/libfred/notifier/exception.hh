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
/**
 *  @file
 */

#ifndef EXCEPTION_HH_153695C519C1436481E3D1BEF1D9E511
#define EXCEPTION_HH_153695C519C1436481E3D1BEF1D9E511

#include <exception>

namespace Notification {

struct ExceptionInterface : std::exception { };

struct ExceptionEventNotImplemented : ExceptionInterface {
    const char* what() const noexcept { return "event not implemented"; }
};

struct ExceptionObjectTypeNotImplemented : ExceptionInterface {
    const char* what() const noexcept { return "object type not yet implemented"; }
};

struct ExceptionAddressTypeNotImplemented : ExceptionInterface {
    const char* what() const noexcept { return "address type not yet implemented"; }
};

struct ExceptionUnknownHistoryId : ExceptionInterface {
    const char* what() const noexcept { return "unknown history id"; }
};

struct ExceptionInvalidUpdateEvent : ExceptionInterface {
    const char* what() const noexcept { return "invalid update event"; }
};

struct ExceptionUnknownEmailTemplate : ExceptionInterface {
    const char* what() const noexcept { return "unknown e-mail template"; }
};

struct ExceptionMissingChangesFlagInUpdateNotificationContent : ExceptionInterface {
    const char* what() const noexcept { return "missing changes flag in update notification content"; }
};

struct ExceptionInvalidNotificationContent : ExceptionInterface {
    const char* what() const noexcept { return "invalid notification content"; }
};

struct ExceptionDataLoss : ExceptionInterface {
    const char* what() const noexcept { return "data loss"; }
};

struct ExceptionUnknownSSNType : ExceptionInterface {
    const char* what() const noexcept { return "unknown SSN type"; }
};

}

#endif
