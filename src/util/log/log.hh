/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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

#include "liblog/liblog.hh"

#include <boost/format.hpp>

template <>
struct fmt::formatter<boost::format>: formatter<std::string>
{
    // parse is inherited from formatter<std::string>.
    template <typename FormatContext>
    auto format(const boost::format& fmt, FormatContext& ctx)
    {
        return formatter<std::string>::format(fmt.str(), ctx);
    }
};

#define FREDLOG_TRACE(msg) LIBLOG_TRACE("{}", (msg))
#define FREDLOG_DEBUG(msg) LIBLOG_DEBUG("{}", (msg))
#define FREDLOG_INFO(msg) LIBLOG_INFO("{}", (msg))
#define FREDLOG_NOTICE(msg) LIBLOG_INFO("{}", (msg))
#define FREDLOG_WARNING(msg) LIBLOG_WARNING("{}", (msg))
#define FREDLOG_ERROR(msg) LIBLOG_ERROR("{}", (msg))
#define FREDLOG_CRITICAL(msg) LIBLOG_CRITICAL("{}", (msg))
#define FREDLOG_ALERT(msg) LIBLOG_CRITICAL("{}", (msg))
#define FREDLOG_EMERG(msg) LIBLOG_CRITICAL("{}", (msg))

#define FREDLOG_SET_CONTEXT(cls, var, ...) LIBLOG_SET_CONTEXT(cls, var, ## __VA_ARGS__)

#endif//LOG_HH_122CBA13F35A4A728F0E7A261062A564
