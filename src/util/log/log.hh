#ifndef LOG_HH_122CBA13F35A4A728F0E7A261062A564
#define LOG_HH_122CBA13F35A4A728F0E7A261062A564

#include <boost/format.hpp>

#include <list>
#include <memory>
#include <string>

namespace Logging {

class Log
{
public:
    Log();
    ~Log();

    enum class EventImportance
    {
        emerg,
        alert,
        crit,
        err,
        warning,
        notice,
        info,
        debug,
        trace
    };

    enum class Device
    {
        console,
        file,
        syslog
    };

    template <Device device, typename ...A>
    Log& add_handler_of(A...);

    Log& set_minimal_importance(EventImportance importance);
    template <EventImportance importance>
    bool is_sufficient()const;

    void trace(const std::string& msg)const;
    void trace(const boost::format& frmt)const;
    void debug(const std::string& msg)const;
    void debug(const boost::format& frmt)const;
    void info(const std::string& msg)const;
    void info(const boost::format& frmt)const;
    void notice(const std::string& msg)const;
    void notice(const boost::format& frmt)const;
    void warning(const std::string& msg)const;
    void warning(const boost::format& frmt)const;
    void error(const std::string& msg)const;
    void error(const boost::format& frmt)const;
    void critical(const std::string& msg)const;
    void critical(const boost::format& frmt)const;
    void alert(const std::string& msg)const;
    void alert(const boost::format& frmt)const;
    void emerg(const std::string& msg)const;
    void emerg(const boost::format& frmt)const;

    /**
     * support for old style formatting log
     */
    void message(EventImportance event_importance, const char* format, ...)const;
    class Handler;
private:
    std::list<std::unique_ptr<Handler>> handlers_;
    EventImportance minimal_importance_;
};

}//namespace Logging

#endif//LOG_HH_122CBA13F35A4A728F0E7A261062A564
