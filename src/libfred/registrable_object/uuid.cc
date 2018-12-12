#include "libfred/registrable_object/uuid.hh"
#include "libfred/registrable_object/contact/contact_uuid.hh"

#include <boost/lexical_cast.hpp>

namespace Util {

std::ostream& operator<<(std::ostream& out, const boost::uuids::uuid& src)
{
    return out << boost::lexical_cast<std::string>(src);
}

}//namespace Util

namespace Database {

namespace {

template <typename U>
U make_uuid_from_database_value(const Value& src)
{
    if (src.isnull())
    {
        struct UnexpectedNullValue : LibFred::RegistrableObject::InvalidUuidString
        {
            const char* what() const noexcept override
            {
                return "unable convert NULL database value to strong type based on boost uuid type";
            }
        };
        throw UnexpectedNullValue();
    }
    return LibFred::RegistrableObject::make_uuid_from_string<U>(src.as<std::string>());
}

}//namespace Database::{anonymous}

template <>
LibFred::RegistrableObject::Contact::ContactUuid Value::as()const
{
    return make_uuid_from_database_value<LibFred::RegistrableObject::Contact::ContactUuid>(*this);
}

template <>
LibFred::RegistrableObject::Contact::ContactHistoryUuid Value::as()const
{
    return make_uuid_from_database_value<LibFred::RegistrableObject::Contact::ContactHistoryUuid>(*this);
}

}//namespace Database

namespace LibFred {
namespace RegistrableObject {

const char* InvalidUuidString::what() const noexcept
{
    return "unable to convert string to uuid";
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred
