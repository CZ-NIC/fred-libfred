#ifndef UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9

#include "util/strong_type.hh"

#include "libfred/object/object_type.hh"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <exception>
#include <iosfwd>
#include <string>
#include <type_traits>

namespace LibFred {
namespace RegistrableObject {

template <typename N>
using StrongTypeUuid = Util::StrongType<boost::uuids::uuid, N>;

struct InvalidUuidString : std::exception
{
    const char* what() const noexcept override;
};

template <typename U>
U make_uuid_from_string(const std::string& src)
{
    try
    {
        static_assert(std::is_same<decltype(get_raw_value_from(U())), boost::uuids::uuid&&>::value,
                      "U must be strong type based on boost uuid");
        return Util::make_strong<U>(boost::uuids::string_generator()(src));
    }
    catch (const std::runtime_error&)
    {
        throw InvalidUuidString();
    }
}

using ObjectUuid = StrongTypeUuid<struct GeneralObjectTag>;

ObjectUuid make_object_uuid(const boost::uuids::uuid& src);
ObjectUuid make_object_uuid(boost::uuids::uuid&& src);
ObjectUuid make_object_uuid(const std::string& src);


using ObjectHistoryUuid = StrongTypeUuid<struct GeneralObjectHistoryTag>;

ObjectHistoryUuid make_object_history_uuid(const boost::uuids::uuid& src);
ObjectHistoryUuid make_object_history_uuid(boost::uuids::uuid&& src);
ObjectHistoryUuid make_object_history_uuid(const std::string& src);


template <Object_Type::Enum>
struct UuidTag;

template <Object_Type::Enum o>
using UuidOf = StrongTypeUuid<UuidTag<o>>;

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const boost::uuids::uuid& src);

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(boost::uuids::uuid&& src);

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const std::string& src);


template <Object_Type::Enum>
struct HistoryUuidTag;

template <Object_Type::Enum o>
using HistoryUuidOf = StrongTypeUuid<HistoryUuidTag<o>>;

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const boost::uuids::uuid& src);

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(boost::uuids::uuid&& src);

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const std::string& src);


template <typename U, typename N>
U uuid_cast(const StrongTypeUuid<N>& src)
{
    return Util::strong_type_cast<U>(src);
}

template <typename U, typename N>
U uuid_cast(StrongTypeUuid<N>&& src)
{
    return Util::strong_type_cast<U>(std::move(src));
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

namespace Util {

std::ostream& operator<<(std::ostream& out, const boost::uuids::uuid& src);

}//namespace Util

#endif//UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9
