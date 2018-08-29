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
};

namespace {

template <Log::EventImportance importance>
bool log_as(const Log& log, const std::list<std::unique_ptr<Log::Handler>>& handlers, const std::string& msg)
{
    if (log.is_sufficient<importance>())
    {
        for (const auto& handler : handlers)
        {
            handler->msg(importance, msg);
        }
        return true;
    }
    return false;
}

template <Log::EventImportance importance>
bool log_as(const Log& log, const std::list<std::unique_ptr<Log::Handler>>& handlers, const boost::format& frmt)
{
    if (log.is_sufficient<importance>())
    {
        if (!handlers.empty())
        {
            const std::string msg = frmt.str();
            for (const auto& handler : handlers)
            {
                handler->msg(importance, msg);
            }
        }
        return true;
    }
    return false;
}

template <Log::EventImportance lhs>
struct Is
{
    static bool less_than(Log::EventImportance rhs)
    {
        const bool lhs_is_numerically_greater_than_rhs = rhs < lhs;
        const bool lhs_is_less_important_than_rhs = lhs_is_numerically_greater_than_rhs;
        return lhs_is_less_important_than_rhs;
    }
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
private:
    virtual std::ostream& get_output_stream() = 0;
    boost::mutex mutex_;
};

class FileHandler : public SharedStreamHandler
{
public:
    explicit FileHandler(const std::string& file_name)
    {
        ofs_.open(file_name, std::ios_base::app);
    }
    ~FileHandler()
    {
        ofs_.flush();
        ofs_.close();
    }
private:
    std::ostream& get_output_stream()override { return ofs_; }
    std::ofstream ofs_;
};

class ConsoleHandler : public SharedStreamHandler
{
public:
    ~ConsoleHandler()
    {
        std::cout.flush();
    }
private:
    std::ostream& get_output_stream()override { return std::cout; }
};

class SysLogHandler : public Log::Handler
{
public:
    explicit SysLogHandler(int local_facility_index)
        : syslog_facility_(get_syslog_facility_local(local_facility_index))
    { }
    ~SysLogHandler() = default;
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
                    case Log::EventImportance::debug: return LOG_DEBUG;
                    case Log::EventImportance::info: return LOG_INFO;
                    case Log::EventImportance::notice: return LOG_NOTICE;
                    case Log::EventImportance::warning: return LOG_WARNING;
                    case Log::EventImportance::err: return LOG_ERR;
                    case Log::EventImportance::crit: return LOG_CRIT;
                    case Log::EventImportance::alert: return LOG_ALERT;
                    case Log::EventImportance::emerg: return LOG_EMERG;
                    case Log::EventImportance::trace: return LOG_DEBUG;
                }
                return LOG_DEBUG;
            }
        };
        syslog(syslog_facility_ | LogLevel::from(event_importance), "%s", (prefix + msg).c_str());
        return *this;
    }
private:
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
};

template <Log::Device device>
std::unique_ptr<Log::Handler> make_log_handler_of();

template <Log::Device device>
std::unique_ptr<Log::Handler> make_log_handler_of(const std::string&);

template <Log::Device device>
std::unique_ptr<Log::Handler> make_log_handler_of(int);

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::file>(const std::string& file_name)
{
    return std::make_unique<FileHandler>(file_name);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::console>()
{
    return std::make_unique<ConsoleHandler>();
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>(int local_facility_index)
{
    return std::make_unique<SysLogHandler>(local_facility_index);
}

template <>
std::unique_ptr<Log::Handler> make_log_handler_of<Log::Device::syslog>()
{
    constexpr int default_local_facility_index = 2;
    return make_log_handler_of<Log::Device::syslog>(default_local_facility_index);
}

}//namespace Logging::{anonymous}

Log::Log()
    : minimal_importance_(EventImportance::info) { }

Log::~Log() { }

template <Log::Device device, typename ...A>
Log& Log::add_handler_of(A...args)
{
    handlers_.push_back(make_log_handler_of<device>(args...));
    return *this;
}

template Log& Log::add_handler_of<Log::Device::file, const std::string&>(const std::string&);
template Log& Log::add_handler_of<Log::Device::file, std::string>(std::string);
template Log& Log::add_handler_of<Log::Device::console>();
template Log& Log::add_handler_of<Log::Device::syslog, int>(int);
template Log& Log::add_handler_of<Log::Device::syslog>();

Log& Log::set_minimal_importance(EventImportance importance)
{
    minimal_importance_ = importance;
    return *this;
}

template <Log::EventImportance event_importance>
bool Log::is_sufficient()const
{
    const bool insufficient_importance = Is<event_importance>::less_than(minimal_importance_);
    return !insufficient_importance;
}

void Log::trace(const std::string& msg)const
{
    log_as<EventImportance::trace>(*this, handlers_, msg);
}

void Log::debug(const std::string& msg)const
{
    log_as<EventImportance::debug>(*this, handlers_, msg);
}

void Log::info(const std::string& msg)const
{
    log_as<EventImportance::info>(*this, handlers_, msg);
}

void Log::notice(const std::string& msg)const
{
    log_as<EventImportance::notice>(*this, handlers_, msg);
}

void Log::warning(const std::string& msg)const
{
    log_as<EventImportance::warning>(*this, handlers_, msg);
}

void Log::error(const std::string& msg)const
{
    log_as<EventImportance::err>(*this, handlers_, msg);
}

void Log::critical(const std::string& msg)const
{
    log_as<EventImportance::crit>(*this, handlers_, msg);
}

void Log::alert(const std::string& msg)const
{
    log_as<EventImportance::alert>(*this, handlers_, msg);
}

void Log::emerg(const std::string& msg)const
{
    log_as<EventImportance::emerg>(*this, handlers_, msg);
}

void Log::trace(const boost::format& frmt)const
{
    log_as<EventImportance::trace>(*this, handlers_, frmt);
}

void Log::debug(const boost::format& frmt)const
{
    log_as<EventImportance::debug>(*this, handlers_, frmt);
}

void Log::info(const boost::format& frmt)const
{
    log_as<EventImportance::info>(*this, handlers_, frmt);
}

void Log::notice(const boost::format& frmt)const
{
    log_as<EventImportance::notice>(*this, handlers_, frmt);
}

void Log::warning(const boost::format& frmt)const
{
    log_as<EventImportance::warning>(*this, handlers_, frmt);
}

void Log::error(const boost::format& frmt)const
{
    log_as<EventImportance::err>(*this, handlers_, frmt);
}

void Log::critical(const boost::format& frmt)const
{
    log_as<EventImportance::crit>(*this, handlers_, frmt);
}

void Log::alert(const boost::format& frmt)const
{
    log_as<EventImportance::alert>(*this, handlers_, frmt);
}

void Log::emerg(const boost::format& frmt)const
{
    log_as<EventImportance::emerg>(*this, handlers_, frmt);
}

void Log::message(EventImportance event_importance, const char* format, ...)const
{
    if (minimal_importance_ < event_importance)
    {
        return;
    }

    char msg_buffer[2048];
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

    for (const auto& handler : handlers_)
    {
        handler->msg(event_importance, msg_buffer);
    }
}

}//namespace Logging
