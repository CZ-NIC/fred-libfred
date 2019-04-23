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

    enum class Severity
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

    class DeviceHandler;
    Log& add_device_handler(std::unique_ptr<DeviceHandler>&& device_handler);

    template <Severity severity>
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
    template <Severity severity_of_logged_event>
    void message(const char* format, ...)const;
private:
    std::list<std::unique_ptr<DeviceHandler>> handlers_;
};

}//namespace Logging

#endif//LOG_HH_122CBA13F35A4A728F0E7A261062A564
