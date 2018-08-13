#ifndef LOG_HH_122CBA13F35A4A728F0E7A261062A564
#define LOG_HH_122CBA13F35A4A728F0E7A261062A564

#include <boost/format.hpp>
#include <boost/any.hpp>

#include <string>
#include <deque>

namespace Logging {

class BaseLogType;

class Log
{
public:
	enum Level
	{
	  LL_EMERG,
	  LL_ALERT,
	  LL_CRIT,
	  LL_ERR,
	  LL_WARNING,
	  LL_NOTICE,
	  LL_INFO,
	  LL_DEBUG,
	  LL_TRACE
	};

	enum Type
	{
	  LT_CONSOLE,
	  LT_FILE,
	  LT_SYSLOG
	};

	Log(const std::string& name);
	~Log();

    void setName(const std::string &name);
    void addHandler(Log::Type type, const boost::any& param = boost::any());

    void setLevel(Log::Level ll);
    Log::Level getLevel() const;

    void trace(const std::string& msg);
    void trace(const boost::format& frmt);
    void debug(const std::string& msg);
    void debug(const boost::format& frmt);
    void info(const std::string& msg);
    void info(const boost::format& frmt);
    void notice(const std::string& msg);
    void notice(const boost::format& frmt);
    void warning(const std::string& msg);
    void warning(const boost::format& frmt);
    void error(const std::string& msg);
    void error(const boost::format& frmt);
    void critical(const std::string& msg);
    void critical(const boost::format& frmt);
    void alert(const std::string& msg);
    void alert(const boost::format& frmt);
    void emerg(const std::string& msg);
    void emerg(const boost::format& frmt);

  /**
   * support for old style formatting log
   */
  void message(int prio, const char *format, ...);
protected:
    std::deque<BaseLogType*> handlers;
    Log::Level level;
    std::string name;
};

}//namespace Logging

#endif//LOG_HH_122CBA13F35A4A728F0E7A261062A564
