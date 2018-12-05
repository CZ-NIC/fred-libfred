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
