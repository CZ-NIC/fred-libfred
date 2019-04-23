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

#ifndef ADD_LOG_DEVICE_HH_5C4847B1B91CAC24F76A5D01C283993E//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define ADD_LOG_DEVICE_HH_5C4847B1B91CAC24F76A5D01C283993E

#include "util/log/log_device.hh"

#include <string>

namespace Logging {

Log& add_file_device(Log& logger, const std::string& file_name, Log::Severity min_severity = Log::Severity::info);
Log& add_console_device(Log& logger, Log::Severity min_severity = Log::Severity::info);
Log& add_syslog_device(Log& logger, int local_facility_index, Log::Severity min_severity = Log::Severity::info);

}//namespace Logging

#endif//ADD_LOG_DEVICE_HH_5C4847B1B91CAC24F76A5D01C283993E
