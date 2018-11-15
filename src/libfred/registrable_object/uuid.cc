#include "libfred/registrable_object/uuid.hh"
#include "libfred/registrable_object/contact/contact_uuid.hh"

namespace Util {
namespace StrongType {

template <>
std::string to_string<boost::uuids::uuid>(const boost::uuids::uuid& src)
{
    return boost::lexical_cast<std::string>(src);
}

}//namespace Util::StrongType
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
