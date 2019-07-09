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

#include "src/util/log/log.hh"
#include "src/util/log/log_device.hh"

#include <stdarg.h>
#include <stdio.h>

namespace Logging {

namespace {

template <Log::Severity severity>
void log_as(const std::list<std::unique_ptr<Log::DeviceHandler>>& handlers, const std::string& msg)
{
    for (const auto& handler : handlers)
    {
        if (handler->is_sufficient(severity))
        {
            handler->msg(severity, msg);
        }
    }
}

template <Log::Severity severity>
void log_as(const std::list<std::unique_ptr<Log::DeviceHandler>>& handlers, const boost::format& frmt)
{
    std::string msg;
    for (const auto& handler : handlers)
    {
        if (handler->is_sufficient(severity))
        {
            if (msg.empty())
            {
                msg = frmt.str();
            }
            handler->msg(severity, msg);
        }
    }
}

}//namespace Logging::{anonymous}

Log::Log() { }

Log::~Log() { }

Log& Log::add_device_handler(std::unique_ptr<DeviceHandler>&& device_handler)
{
    handlers_.push_back(std::move(device_handler));
    return *this;
}

template <Log::Severity severity_of_logged_event>
bool Log::is_sufficient()const
{
    for (const auto& handler : handlers_)
    {
        if (handler->is_sufficient(severity_of_logged_event))
        {
            return true;
        }
    }
    return false;
}

template bool Log::is_sufficient<Log::Severity::trace>()const;
template bool Log::is_sufficient<Log::Severity::debug>()const;
template bool Log::is_sufficient<Log::Severity::info>()const;
template bool Log::is_sufficient<Log::Severity::notice>()const;
template bool Log::is_sufficient<Log::Severity::warning>()const;
template bool Log::is_sufficient<Log::Severity::err>()const;
template bool Log::is_sufficient<Log::Severity::crit>()const;
template bool Log::is_sufficient<Log::Severity::alert>()const;
template bool Log::is_sufficient<Log::Severity::emerg>()const;

void Log::trace(const std::string& msg)const
{
    log_as<Severity::trace>(handlers_, msg);
}

void Log::debug(const std::string& msg)const
{
    log_as<Severity::debug>(handlers_, msg);
}

void Log::info(const std::string& msg)const
{
    log_as<Severity::info>(handlers_, msg);
}

void Log::notice(const std::string& msg)const
{
    log_as<Severity::notice>(handlers_, msg);
}

void Log::warning(const std::string& msg)const
{
    log_as<Severity::warning>(handlers_, msg);
}

void Log::error(const std::string& msg)const
{
    log_as<Severity::err>(handlers_, msg);
}

void Log::critical(const std::string& msg)const
{
    log_as<Severity::crit>(handlers_, msg);
}

void Log::alert(const std::string& msg)const
{
    log_as<Severity::alert>(handlers_, msg);
}

void Log::emerg(const std::string& msg)const
{
    log_as<Severity::emerg>(handlers_, msg);
}

void Log::trace(const boost::format& frmt)const
{
    log_as<Severity::trace>(handlers_, frmt);
}

void Log::debug(const boost::format& frmt)const
{
    log_as<Severity::debug>(handlers_, frmt);
}

void Log::info(const boost::format& frmt)const
{
    log_as<Severity::info>(handlers_, frmt);
}

void Log::notice(const boost::format& frmt)const
{
    log_as<Severity::notice>(handlers_, frmt);
}

void Log::warning(const boost::format& frmt)const
{
    log_as<Severity::warning>(handlers_, frmt);
}

void Log::error(const boost::format& frmt)const
{
    log_as<Severity::err>(handlers_, frmt);
}

void Log::critical(const boost::format& frmt)const
{
    log_as<Severity::crit>(handlers_, frmt);
}

void Log::alert(const boost::format& frmt)const
{
    log_as<Severity::alert>(handlers_, frmt);
}

void Log::emerg(const boost::format& frmt)const
{
    log_as<Severity::emerg>(handlers_, frmt);
}

template <Log::Severity severity_of_logged_event>
void Log::message(const char* format, ...)const
{
    char msg_buffer[2048];
    msg_buffer[0] = '\0';
    for (const auto& handler : handlers_)
    {
        if (handler->is_sufficient(severity_of_logged_event))
        {
            if (msg_buffer[0] == '\0')
            {
                va_list args;
                va_start(args, format);
                const int buffer_capacity = sizeof(msg_buffer) - 1;
                const auto bytes = vsnprintf(msg_buffer, buffer_capacity, format, args);
                va_end(args);
                const bool output_was_truncated = buffer_capacity <= bytes;
                if (output_was_truncated)
                {
                    msg_buffer[buffer_capacity] = '\0';
                }
            }
            handler->msg(severity_of_logged_event, msg_buffer);
        }
    }
}

template void Log::message<Log::Severity::trace>(const char*, ...)const;
template void Log::message<Log::Severity::debug>(const char*, ...)const;
template void Log::message<Log::Severity::info>(const char*, ...)const;
template void Log::message<Log::Severity::notice>(const char*, ...)const;
template void Log::message<Log::Severity::warning>(const char*, ...)const;
template void Log::message<Log::Severity::err>(const char*, ...)const;
template void Log::message<Log::Severity::crit>(const char*, ...)const;
template void Log::message<Log::Severity::alert>(const char*, ...)const;
template void Log::message<Log::Severity::emerg>(const char*, ...)const;

}//namespace Logging
