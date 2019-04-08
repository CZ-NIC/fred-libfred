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
    virtual Handler& msg(Log::EventImportance event_importance, const std::string& msg) = 0;
    virtual bool is_sufficient(Log::EventImportance)const = 0;
};

namespace {

template <Log::EventImportance importance>
void log_as(const std::list<std::unique_ptr<Log::Handler>>& handlers, const std::string& msg)
{
    for (const auto& handler : handlers)
    {
        if (handler->is_sufficient(importance))
        {
            handler->msg(importance, msg);
        }
    }
}

template <Log::EventImportance importance>
void log_as(const std::list<std::unique_ptr<Log::Handler>>& handlers, const boost::format& frmt)
{
    std::string msg;
    for (const auto& handler : handlers)
    {
        if (handler->is_sufficient(importance))
        {
            if (msg.empty())
            {
                msg = frmt.str();
            }
            handler->msg(importance, msg);
        }
    }
}

class Is
{
public:
    explicit Is(Log::EventImportance lhs)
        : lhs_(lhs)
    { }
    bool less_important_than(Log::EventImportance rhs)const
    {
        const bool lhs_is_numerically_greater_than_rhs = rhs < lhs_;
        const bool lhs_is_less_important_than_rhs = lhs_is_numerically_greater_than_rhs;
        return lhs_is_less_important_than_rhs;
    }
private:
    const Log::EventImportance lhs_;
};

std::string importance2str(Log::EventImportance event_importance)
{
    switch (event_importance)
    {
        case Log::EventImportance::debug: return "debug";
        case Log::EventImportance::info: return "info";
        case Log::EventImportance::notice: return "notice";
        case Log::EventImportance::warning: return "warning";
        case Log::EventImportance::err: return "error";
        case Log::EventImportance::crit: return "critical";
        case Log::EventImportance::alert: return "alert";
        case Log::EventImportance::emerg: return "emerg";
        case Log::EventImportance::trace: return "trace";
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
    Handler& msg(Log::EventImportance event_importance, const std::string& msg)override
    {
        const auto str_now = get_current_time();
        const auto context = Context::get();
        const auto importance = importance2str(event_importance);
        auto& out = this->get_output_stream();
        boost::mutex::scoped_lock scoped_lock(mutex_);

        if (!context.empty())
        {
            out << boost::format("[%1%] %|23t|[%2%] %|34t|[%3%] -- %4%") %
                   str_now %
                   importance %
                   context %
                   msg;
        }
        else
        {
            out << boost::format("[%1%] %|23t|[%2%] %|34t|%3%") %
                   str_now %
                   importance %
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
    explicit FileHandler(const std::string& file_name, Log::EventImportance min_importance = Log::EventImportance::info)
        : min_importance_(min_importance)
    {
        ofs_.open(file_name, std::ios_base::app);
    }
    ~FileHandler()
    {
        ofs_.flush();
        ofs_.close();
    }
private:
    bool is_sufficient(Log::EventImportance event_importance)const override
    {
        const bool insufficient_importance = Is(event_importance).less_important_than(min_importance_);
        return !insufficient_importance;
    }
    std::ostream& get_output_stream()override { return ofs_; }
    std::ofstream ofs_;
    const Log::EventImportance min_importance_;
};

class ConsoleHandler : public SharedStreamHandler
{
public:
    ConsoleHandler(Log::EventImportance min_importance = Log::EventImportance::info)
        : min_importance_(min_importance)
    {
    }
    ~ConsoleHandler()
    {
        std::cout.flush();
    }
private:
    bool is_sufficient(Log::EventImportance event_importance)const override
    {
        const bool insufficient_importance = Is(event_importance).less_important_than(min_importance_);
        return !insufficient_importance;
    }
    std::ostream& get_output_stream()override { return std::cout; }
    const Log::EventImportance min_importance_;
};

class SysLogHandler : public Log::Handler
{
public:
    explicit SysLogHandler(int local_facility_index, Log::EventImportance min_importance = Log::EventImportance::info)
        : syslog_facility_(get_syslog_facility_local(local_facility_index)),
          min_importance_(min_importance)
    { }
    ~SysLogHandler() = default;
private:
    Handler& msg(Log::EventImportance event_importance, const std::string& msg)override
    {
        const auto context = Context::get();
        const std::string prefix = (context.empty() ? "" : "[" + context + "] -- ");
        struct LogLevel
        {
            static int from(Log::EventImportance event_importance)
            {
                switch (event_importance)
                {
                    case Log::EventImportance::trace://the same as debug
                    case Log::EventImportance::debug: return LOG_DEBUG;
                    case Log::EventImportance::info: return LOG_INFO;
                    case Log::EventImportance::notice: return LOG_NOTICE;
                    case Log::EventImportance::warning: return LOG_WARNING;
                    case Log::EventImportance::err: return LOG_ERR;
                    case Log::EventImportance::crit: return LOG_CRIT;
                    case Log::EventImportance::alert: return LOG_ALERT;
                    case Log::EventImportance::emerg: return LOG_EMERG;
                }
                return LOG_DEBUG;
            }
        };
        syslog(syslog_facility_ | LogLevel::from(event_importance), "%s", (prefix + msg).c_str());
        return *this;
    }
    bool is_sufficient(Log::EventImportance event_importance)const override
    {
        const bool insufficient_importance = Is(event_importance).less_important_than(min_importance_);
        return !insufficient_importance;
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
    const Log::EventImportance min_importance_;
};

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of();

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(Log::EventImportance);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(const std::string&);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(const std::string&, Log::EventImportance);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(int);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(Log::EventImportance);

template <Log::Device>
std::unique_ptr<Log::Handler> make_log_handler_of(int, Log::EventImportance);

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::file>(const std::string& file_name)
{
    return std::make_unique<FileHandler>(file_name, Log::EventImportance::info);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::file>(
        const std::string& file_name,
        Log::EventImportance min_importance)
{
    return std::make_unique<FileHandler>(file_name, min_importance);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::console>()
{
    return std::make_unique<ConsoleHandler>(Log::EventImportance::info);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::console>(Log::EventImportance min_importance)
{
    return std::make_unique<ConsoleHandler>(min_importance);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(
        int local_facility_index,
        Log::EventImportance min_importance)
{
    return std::make_unique<SysLogHandler>(local_facility_index, min_importance);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(int local_facility_index)
{
    constexpr Log::EventImportance default_min_importance = Log::EventImportance::info;
    return make_log_handler_of<Log::Device::syslog>(local_facility_index, default_min_importance);
}

constexpr int default_local_facility_index = 2;

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(Log::EventImportance min_importance)
{
    return make_log_handler_of<Log::Device::syslog>(default_local_facility_index, min_importance);
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
template Log& Log::add_handler_of<Log::Device::file, const char*>(const char*, EventImportance);
template Log& Log::add_handler_of<Log::Device::file, std::string>(std::string);
template Log& Log::add_handler_of<Log::Device::file, std::string>(std::string, EventImportance);
template Log& Log::add_handler_of<Log::Device::console>();
template Log& Log::add_handler_of<Log::Device::console>(EventImportance);
template Log& Log::add_handler_of<Log::Device::syslog, int>(int);
template Log& Log::add_handler_of<Log::Device::syslog, unsigned>(unsigned);
template Log& Log::add_handler_of<Log::Device::syslog>(EventImportance);
template Log& Log::add_handler_of<Log::Device::syslog, int>(int, EventImportance);
template Log& Log::add_handler_of<Log::Device::syslog, unsigned>(unsigned, EventImportance);
template Log& Log::add_handler_of<Log::Device::syslog>();

template <Log::EventImportance event_importance>
bool Log::is_sufficient()const
{
    for (const auto& handler : handlers_)
    {
        if (handler->is_sufficient(event_importance))
        {
            return true;
        }
    }
    return false;
}

template bool Log::is_sufficient<Log::EventImportance::trace>()const;
template bool Log::is_sufficient<Log::EventImportance::debug>()const;
template bool Log::is_sufficient<Log::EventImportance::info>()const;
template bool Log::is_sufficient<Log::EventImportance::notice>()const;
template bool Log::is_sufficient<Log::EventImportance::warning>()const;
template bool Log::is_sufficient<Log::EventImportance::err>()const;
template bool Log::is_sufficient<Log::EventImportance::crit>()const;
template bool Log::is_sufficient<Log::EventImportance::alert>()const;
template bool Log::is_sufficient<Log::EventImportance::emerg>()const;

void Log::trace(const std::string& msg)const
{
    log_as<EventImportance::trace>(handlers_, msg);
}

void Log::debug(const std::string& msg)const
{
    log_as<EventImportance::debug>(handlers_, msg);
}

void Log::info(const std::string& msg)const
{
    log_as<EventImportance::info>(handlers_, msg);
}

void Log::notice(const std::string& msg)const
{
    log_as<EventImportance::notice>(handlers_, msg);
}

void Log::warning(const std::string& msg)const
{
    log_as<EventImportance::warning>(handlers_, msg);
}

void Log::error(const std::string& msg)const
{
    log_as<EventImportance::err>(handlers_, msg);
}

void Log::critical(const std::string& msg)const
{
    log_as<EventImportance::crit>(handlers_, msg);
}

void Log::alert(const std::string& msg)const
{
    log_as<EventImportance::alert>(handlers_, msg);
}

void Log::emerg(const std::string& msg)const
{
    log_as<EventImportance::emerg>(handlers_, msg);
}

void Log::trace(const boost::format& frmt)const
{
    log_as<EventImportance::trace>(handlers_, frmt);
}

void Log::debug(const boost::format& frmt)const
{
    log_as<EventImportance::debug>(handlers_, frmt);
}

void Log::info(const boost::format& frmt)const
{
    log_as<EventImportance::info>(handlers_, frmt);
}

void Log::notice(const boost::format& frmt)const
{
    log_as<EventImportance::notice>(handlers_, frmt);
}

void Log::warning(const boost::format& frmt)const
{
    log_as<EventImportance::warning>(handlers_, frmt);
}

void Log::error(const boost::format& frmt)const
{
    log_as<EventImportance::err>(handlers_, frmt);
}

void Log::critical(const boost::format& frmt)const
{
    log_as<EventImportance::crit>(handlers_, frmt);
}

void Log::alert(const boost::format& frmt)const
{
    log_as<EventImportance::alert>(handlers_, frmt);
}

void Log::emerg(const boost::format& frmt)const
{
    log_as<EventImportance::emerg>(handlers_, frmt);
}

template <Log::EventImportance event_importance>
void Log::message(const char* format, ...)const
{
    char msg_buffer[2048];
    msg_buffer[0] = '\0';
    for (const auto& handler : handlers_)
    {
        if (handler->is_sufficient(event_importance))
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
            handler->msg(event_importance, msg_buffer);
        }
    }
}

template void Log::message<Log::EventImportance::trace>(const char*, ...)const;
template void Log::message<Log::EventImportance::debug>(const char*, ...)const;
template void Log::message<Log::EventImportance::info>(const char*, ...)const;
template void Log::message<Log::EventImportance::notice>(const char*, ...)const;
template void Log::message<Log::EventImportance::warning>(const char*, ...)const;
template void Log::message<Log::EventImportance::err>(const char*, ...)const;
template void Log::message<Log::EventImportance::crit>(const char*, ...)const;
template void Log::message<Log::EventImportance::alert>(const char*, ...)const;
template void Log::message<Log::EventImportance::emerg>(const char*, ...)const;

}//namespace Logging
