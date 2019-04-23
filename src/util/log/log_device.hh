/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#ifndef LOG_DEVICE_HH_129D461866BECCB9B86AF4BE272CC2F6//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define LOG_DEVICE_HH_129D461866BECCB9B86AF4BE272CC2F6

#include "util/log/log.hh"

#include <string>

namespace Logging {

class Log::DeviceHandler
{
public:
    virtual ~DeviceHandler() = default;
    virtual DeviceHandler& msg(Log::Severity severity_of_logged_event, const std::string& msg) = 0;
    virtual bool is_sufficient(Log::Severity)const = 0;
};

}//namespace Logging

#endif//LOG_DEVICE_HH_129D461866BECCB9B86AF4BE272CC2F6
