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
#ifndef LOGGER_HH_FF92F982C11C49949EC827AE9795E29A
#define LOGGER_HH_FF92F982C11C49949EC827AE9795E29A

#include "config.h"

#ifdef HAVE_LOGGER

#include "util/log/log.hh"
#include "util/singleton.hh"

namespace Logging {

typedef Singleton<Log> Manager;

}//namespace Logging

#define LOGGER Logging::Manager::instance_ref()
#define TRACE(msg) LOGGER.trace(msg)

#define LOG LOGGER.message

#endif//HAVE_LOGGER

#endif//LOGGER_HH_FF92F982C11C49949EC827AE9795E29A
