#include "libfred/registrable_object/uuid.hh"
#include "libfred/registrable_object/contact/contact_uuid.hh"

#include <boost/lexical_cast.hpp>

#include <iostream>

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

ObjectUuid make_object_uuid(const boost::uuids::uuid& src)
{
    return Util::make_strong<ObjectUuid>(src);
}

ObjectUuid make_object_uuid(boost::uuids::uuid&& src)
{
    return Util::make_strong<ObjectUuid>(std::move(src));
}

ObjectUuid make_object_uuid(const std::string& src)
{
    return make_uuid_from_string<ObjectUuid>(src);
}


ObjectHistoryUuid make_object_history_uuid(const boost::uuids::uuid& src)
{
    return Util::make_strong<ObjectHistoryUuid>(src);
}

ObjectHistoryUuid make_object_history_uuid(boost::uuids::uuid&& src)
{
    return Util::make_strong<ObjectHistoryUuid>(std::move(src));
}

ObjectHistoryUuid make_object_history_uuid(const std::string& src)
{
    return make_uuid_from_string<ObjectHistoryUuid>(src);
}


template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const boost::uuids::uuid& src)
{
    return Util::make_strong<UuidOf<o>>(src);
}

template UuidOf<Object_Type::contact> make_uuid_of<Object_Type::contact>(const boost::uuids::uuid&);
template UuidOf<Object_Type::domain> make_uuid_of<Object_Type::domain>(const boost::uuids::uuid&);
template UuidOf<Object_Type::keyset> make_uuid_of<Object_Type::keyset>(const boost::uuids::uuid&);
template UuidOf<Object_Type::nsset> make_uuid_of<Object_Type::nsset>(const boost::uuids::uuid&);


template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(boost::uuids::uuid&& src)
{
    return Util::make_strong<UuidOf<o>>(std::move(src));
}

template UuidOf<Object_Type::contact> make_uuid_of<Object_Type::contact>(boost::uuids::uuid&&);
template UuidOf<Object_Type::domain> make_uuid_of<Object_Type::domain>(boost::uuids::uuid&&);
template UuidOf<Object_Type::keyset> make_uuid_of<Object_Type::keyset>(boost::uuids::uuid&&);
template UuidOf<Object_Type::nsset> make_uuid_of<Object_Type::nsset>(boost::uuids::uuid&&);


template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const std::string& src)
{
    return make_uuid_from_string<UuidOf<o>>(src);
}

template UuidOf<Object_Type::contact> make_uuid_of<Object_Type::contact>(const std::string&);
template UuidOf<Object_Type::domain> make_uuid_of<Object_Type::domain>(const std::string&);
template UuidOf<Object_Type::keyset> make_uuid_of<Object_Type::keyset>(const std::string&);
template UuidOf<Object_Type::nsset> make_uuid_of<Object_Type::nsset>(const std::string&);


template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const boost::uuids::uuid& src)
{
    return Util::make_strong<HistoryUuidOf<o>>(src);
}

template HistoryUuidOf<Object_Type::contact> make_history_uuid_of<Object_Type::contact>(const boost::uuids::uuid&);
template HistoryUuidOf<Object_Type::domain> make_history_uuid_of<Object_Type::domain>(const boost::uuids::uuid&);
template HistoryUuidOf<Object_Type::keyset> make_history_uuid_of<Object_Type::keyset>(const boost::uuids::uuid&);
template HistoryUuidOf<Object_Type::nsset> make_history_uuid_of<Object_Type::nsset>(const boost::uuids::uuid&);


template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(boost::uuids::uuid&& src)
{
    return Util::make_strong<HistoryUuidOf<o>>(std::move(src));
}

template HistoryUuidOf<Object_Type::contact> make_history_uuid_of<Object_Type::contact>(boost::uuids::uuid&&);
template HistoryUuidOf<Object_Type::domain> make_history_uuid_of<Object_Type::domain>(boost::uuids::uuid&&);
template HistoryUuidOf<Object_Type::keyset> make_history_uuid_of<Object_Type::keyset>(boost::uuids::uuid&&);
template HistoryUuidOf<Object_Type::nsset> make_history_uuid_of<Object_Type::nsset>(boost::uuids::uuid&&);


template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const std::string& src)
{
    return make_uuid_from_string<HistoryUuidOf<o>>(src);
}

template HistoryUuidOf<Object_Type::contact> make_history_uuid_of<Object_Type::contact>(const std::string&);
template HistoryUuidOf<Object_Type::domain> make_history_uuid_of<Object_Type::domain>(const std::string&);
template HistoryUuidOf<Object_Type::keyset> make_history_uuid_of<Object_Type::keyset>(const std::string&);
template HistoryUuidOf<Object_Type::nsset> make_history_uuid_of<Object_Type::nsset>(const std::string&);

}//namespace LibFred::RegistrableObject
}//namespace LibFred
