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
#include "src/util/log/log_device.hh"
#include "src/util/log/context.hh"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>

#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>

#include <iostream>
#include <fstream>

namespace Logging {

namespace {

struct FilterLimit
{
    explicit FilterLimit(Log::Severity severity) : value(severity) { }
    const Log::Severity value;
};

class LowerSeverityIsFilteredOut
{
public:
    explicit LowerSeverityIsFilteredOut(FilterLimit limit)
        : limit_(limit.value)
    { }
    bool is_important_enough(Log::Severity severity_of_logged_event)const noexcept
    {
        const bool insufficient_severity = LessImportant()(severity_of_logged_event, limit_);
        return !insufficient_severity;
    }
private:
    const Log::Severity limit_;
    struct LessImportant
    {
        bool operator()(Log::Severity lhs, Log::Severity rhs)const noexcept
        {
            const bool lhs_is_numerically_greater_than_rhs = rhs < lhs;
            const bool lhs_is_less_important_than_rhs = lhs_is_numerically_greater_than_rhs;
            return lhs_is_less_important_than_rhs;
        }
    };
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

class SharedStreamHandler : public Log::DeviceHandler
{
public:
    ~SharedStreamHandler() = default;
private:
    DeviceHandler& msg(Log::Severity severity_of_logged_event, const std::string& msg)override
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
        return LowerSeverityIsFilteredOut(FilterLimit(min_severity_)).is_important_enough(severity_of_logged_event);
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
        return LowerSeverityIsFilteredOut(FilterLimit(min_severity_)).is_important_enough(severity_of_logged_event);
    }
    std::ostream& get_output_stream()override { return std::cout; }
    const Log::Severity min_severity_;
};

class SysLogHandler : public Log::DeviceHandler
{
public:
    explicit SysLogHandler(int local_facility_index, Log::Severity min_severity = Log::Severity::info)
        : syslog_facility_(get_syslog_facility_local(local_facility_index)),
          min_severity_(min_severity)
    { }
    ~SysLogHandler() = default;
private:
    DeviceHandler& msg(Log::Severity severity_of_logged_event, const std::string& msg)override
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
        return LowerSeverityIsFilteredOut(FilterLimit(min_severity_)).is_important_enough(severity_of_logged_event);
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

}//namespace Logging::{anonymous}

Log& add_file_device(Log& logger, const std::string& file_name, Log::Severity min_severity)
{
    return logger.add_device_handler(std::make_unique<FileHandler>(file_name, min_severity));
}

Log& add_console_device(Log& logger, Log::Severity min_severity)
{
    return logger.add_device_handler(std::make_unique<ConsoleHandler>(min_severity));
}

Log& add_syslog_device(Log& logger, int local_facility_index, Log::Severity min_severity)
{
    return logger.add_device_handler(std::make_unique<SysLogHandler>(local_facility_index, min_severity));
}

}//namespace Logging
