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
#include "util/log/log.hh"
#include "util/log/context.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>

#include <iostream>
#include <fstream>

namespace Logging {

class Log::Handler
{
public:
    virtual ~Handler() = default;
    virtual Handler& msg(Log::Severity severity_of_logged_event, const std::string& msg) = 0;
    virtual bool is_sufficient(Log::Severity)const = 0;
};

namespace {

template <Log::Severity severity>
void log_as(const std::list<std::unique_ptr<Log::Handler>>& handlers, const std::string& msg)
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
void log_as(const std::list<std::unique_ptr<Log::Handler>>& handlers, const boost::format& frmt)
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

class Is
{
public:
    explicit Is(Log::Severity lhs)
        : lhs_(lhs)
    { }
    bool less_important_than(Log::Severity rhs)const
    {
        const bool lhs_is_numerically_greater_than_rhs = rhs < lhs_;
        const bool lhs_is_less_important_than_rhs = lhs_is_numerically_greater_than_rhs;
        return lhs_is_less_important_than_rhs;
    }
private:
    const Log::Severity lhs_;
};

std::string severity2str(Log::Severity severity_of_logged_event)
{
    switch (severity_of_logged_event)
    {
        case Log::Severity::debug: return "debug";
        case Log::Severity::info: return "info";
        case Log::Severity::notice: return "notice";
        case Log::Severity::warning: return "warning";
        case Log::Severity::err: return "error";
        case Log::Severity::crit: return "critical";
        case Log::Severity::alert: return "alert";
        case Log::Severity::emerg: return "emerg";
        case Log::Severity::trace: return "trace";
    }
    return "unknown";
}

std::string get_current_time()
{
    try
    {
        const boost::posix_time::ptime now = boost::posix_time::ptime(boost::posix_time::second_clock::local_time());
        return boost::posix_time::to_simple_string(now);
    }
    catch (...)
    {
        // Valgrind throws exception even if everything is ok.
        std::cerr << "ERROR: boost posix time library" << std::endl;
    }
    return std::string();
}

class SharedStreamHandler : public Log::Handler
{
public:
    ~SharedStreamHandler() = default;
private:
    Handler& msg(Log::Severity severity_of_logged_event, const std::string& msg)override
    {
        const auto str_now = get_current_time();
        const auto context = Context::get();
        const auto severity = severity2str(severity_of_logged_event);
        auto& out = this->get_output_stream();
        boost::mutex::scoped_lock scoped_lock(mutex_);

        if (!context.empty())
        {
            out << boost::format("[%1%] %|23t|[%2%] %|34t|[%3%] -- %4%") %
                   str_now %
                   severity %
                   context %
                   msg;
        }
        else
        {
            out << boost::format("[%1%] %|23t|[%2%] %|34t|%3%") %
                   str_now %
                   severity %
                   msg;
        }
        out << std::endl;
        return *this;
    }
    virtual std::ostream& get_output_stream() = 0;
    boost::mutex mutex_;
};

class FileHandler : public SharedStreamHandler
{
public:
    explicit FileHandler(const std::string& file_name, Log::Severity min_severity = Log::Severity::info)
        : min_severity_(min_severity)
    {
        ofs_.open(file_name, std::ios_base::app);
    }
    ~FileHandler()
    {
        ofs_.flush();
        ofs_.close();
    }
private:
    bool is_sufficient(Log::Severity severity_of_logged_event)const override
    {
        const bool insufficient_severity = Is(severity_of_logged_event).less_important_than(min_severity_);
        return !insufficient_severity;
    }
    std::ostream& get_output_stream()override { return ofs_; }
    std::ofstream ofs_;
    const Log::Severity min_severity_;
};

class ConsoleHandler : public SharedStreamHandler
{
public:
    ConsoleHandler(Log::Severity min_severity = Log::Severity::info)
        : min_severity_(min_severity)
    {
    }
    ~ConsoleHandler()
    {
        std::cout.flush();
    }
private:
    bool is_sufficient(Log::Severity severity_of_logged_event)const override
    {
        const bool insufficient_severity = Is(severity_of_logged_event).less_important_than(min_severity_);
        return !insufficient_severity;
    }
    std::ostream& get_output_stream()override { return std::cout; }
    const Log::Severity min_severity_;
};

class SysLogHandler : public Log::Handler
{
public:
    explicit SysLogHandler(int local_facility_index, Log::Severity min_severity = Log::Severity::info)
        : syslog_facility_(get_syslog_facility_local(local_facility_index)),
          min_severity_(min_severity)
    { }
    ~SysLogHandler() = default;
private:
    Handler& msg(Log::Severity severity_of_logged_event, const std::string& msg)override
    {
        const auto context = Context::get();
        const std::string prefix = (context.empty() ? "" : "[" + context + "] -- ");
        struct LogLevel
        {
            static int from(Log::Severity severity_of_logged_event)
            {
                switch (severity_of_logged_event)
                {
                    case Log::Severity::trace://the same as debug
                    case Log::Severity::debug: return LOG_DEBUG;
                    case Log::Severity::info: return LOG_INFO;
                    case Log::Severity::notice: return LOG_NOTICE;
                    case Log::Severity::warning: return LOG_WARNING;
                    case Log::Severity::err: return LOG_ERR;
                    case Log::Severity::crit: return LOG_CRIT;
                    case Log::Severity::alert: return LOG_ALERT;
                    case Log::Severity::emerg: return LOG_EMERG;
                }
                return LOG_DEBUG;
            }
        };
        syslog(syslog_facility_ | LogLevel::from(severity_of_logged_event), "%s", (prefix + msg).c_str());
        return *this;
    }
    bool is_sufficient(Log::Severity severity_of_logged_event)const override
    {
        const bool insufficient_severity = Is(severity_of_logged_event).less_important_than(min_severity_);
        return !insufficient_severity;
    }
    static int get_syslog_facility_local(int index)
    {
        constexpr int the_number_of_local_facilities = 8;
        switch (index % the_number_of_local_facilities)
        {
            case 0: return LOG_LOCAL0;
            case 1: return LOG_LOCAL1;
            case 2: return LOG_LOCAL2;
            case 3: return LOG_LOCAL3;
            case 4: return LOG_LOCAL4;
            case 5: return LOG_LOCAL5;
            case 6: return LOG_LOCAL6;
            case 7: return LOG_LOCAL7;
        }
        return LOG_LOCAL0;
    }
    const int syslog_facility_;
    const Log::Severity min_severity_;
};

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of();

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(Log::Severity);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(const std::string&);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(const std::string&, Log::Severity);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(int);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(Log::Severity);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(int, Log::Severity);

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::file>(const std::string& file_name)
{
    return std::make_unique<FileHandler>(file_name, Log::Severity::info);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::file>(
        const std::string& file_name,
        Log::Severity min_severity)
{
    return std::make_unique<FileHandler>(file_name, min_severity);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::console>()
{
    return std::make_unique<ConsoleHandler>(Log::Severity::info);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::console>(Log::Severity min_severity)
{
    return std::make_unique<ConsoleHandler>(min_severity);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(
        int local_facility_index,
        Log::Severity min_severity)
{
    return std::make_unique<SysLogHandler>(local_facility_index, min_severity);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(int local_facility_index)
{
    constexpr Log::Severity default_min_severity = Log::Severity::info;
    return make_log_handler_of<Log::Device::syslog>(local_facility_index, default_min_severity);
}

constexpr int default_local_facility_index = 2;

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(Log::Severity min_severity)
{
    return make_log_handler_of<Log::Device::syslog>(default_local_facility_index, min_severity);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>()
{
    return make_log_handler_of<Log::Device::syslog>(default_local_facility_index);
}

}//namespace Logging::{anonymous}

Log::Log() { }

Log::~Log() { }

template <Log::Device device, typename ...A>
Log& Log::add_handler_of(A...args)
{
    handlers_.push_back(make_log_handler_of<device>(args...));
    return *this;
}

template Log& Log::add_handler_of<Log::Device::file, const char*>(const char*);
template Log& Log::add_handler_of<Log::Device::file, const char*>(const char*, Severity);
template Log& Log::add_handler_of<Log::Device::file, std::string>(std::string);
template Log& Log::add_handler_of<Log::Device::file, std::string>(std::string, Severity);
template Log& Log::add_handler_of<Log::Device::console>();
template Log& Log::add_handler_of<Log::Device::console>(Severity);
template Log& Log::add_handler_of<Log::Device::syslog, int>(int);
template Log& Log::add_handler_of<Log::Device::syslog, unsigned>(unsigned);
template Log& Log::add_handler_of<Log::Device::syslog>(Severity);
template Log& Log::add_handler_of<Log::Device::syslog, int>(int, Severity);
template Log& Log::add_handler_of<Log::Device::syslog, unsigned>(unsigned, Severity);
template Log& Log::add_handler_of<Log::Device::syslog>();

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
