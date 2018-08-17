#ifndef LOGGER_CLIENT_HH_6295BB642CBC4154A1BA7AEC088EF657
#define LOGGER_CLIENT_HH_6295BB642CBC4154A1BA7AEC088EF657

#include "src/libfred/requests/request_manager.hh"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <memory>
#include <string>

namespace LibFred {
namespace Logger {

class LoggerClient
{
public:
    virtual ~LoggerClient() { }

    virtual unsigned long long getRequestCount(
            const boost::posix_time::ptime& from,
            const boost::posix_time::ptime& to,
            const std::string& service,
            const std::string& user) = 0;

    virtual std::unique_ptr<RequestCountInfo> getRequestCountUsers(
            const boost::posix_time::ptime& from,
            const boost::posix_time::ptime& to,
            const std::string& service) = 0;

    virtual unsigned long long createRequest(
            const std::string& src_ip,
            const std::string& service,
            const std::string& content,
            const RequestProperties& properties,
            const ObjectReferences& references,
            const std::string& type,
            unsigned long long session_id) = 0;

    virtual void closeRequest(
            unsigned long long request_id,
            const std::string& service,
            const std::string& content,
            const RequestProperties& properties,
            const ObjectReferences& references,
            const std::string& result,
            unsigned long long session_id) = 0;
};

}//namespace LibFred::Logger
}//namespace LibFred

#endif//LOGGER_CLIENT_HH_6295BB642CBC4154A1BA7AEC088EF657
