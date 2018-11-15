#ifndef UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9

#include "util/strong_type/named.hh"

#include "libfred/object/object_type.hh"

#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <exception>
#include <string>
#include <type_traits>

namespace LibFred {
namespace RegistrableObject {

template <typename N>
using NamedUuid = Util::StrongType::Named<boost::uuids::uuid, N>;

struct InvalidUuidString : std::exception
{
    const char* what() const noexcept override { return "unable to convert string to uuid"; }
};

template <typename U>
U make_uuid_from_string(const std::string& src)
{
    try
    {
        static_assert(std::is_same<decltype(get_raw_value_from(U())), const boost::uuids::uuid&>::value,
                      "U must be strong type based on boost uuid");
        return Util::StrongType::make<U>(boost::uuids::string_generator()(src));
    }
    catch (const std::runtime_error&)
    {
        throw InvalidUuidString();
    }
}

template <Object_Type::Enum>
struct UuidTag;

template <Object_Type::Enum o>
using UuidOf = NamedUuid<UuidTag<o>>;

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const boost::uuids::uuid& src)
{
    return Util::StrongType::make<UuidOf<o>>(src);
}

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(boost::uuids::uuid&& src)
{
    return Util::StrongType::make<UuidOf<o>>(std::move(src));
}

template <Object_Type::Enum o>
UuidOf<o> make_uuid_of(const std::string& src)
{
    return make_uuid_from_string<UuidOf<o>>(src);
}

template <Object_Type::Enum>
struct HistoryUuidTag;

template <Object_Type::Enum o>
using HistoryUuidOf = NamedUuid<HistoryUuidTag<o>>;

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const boost::uuids::uuid& src)
{
    return Util::StrongType::make<HistoryUuidOf<o>>(src);
}

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(boost::uuids::uuid&& src)
{
    return Util::StrongType::make<HistoryUuidOf<o>>(std::move(src));
}

template <Object_Type::Enum o>
HistoryUuidOf<o> make_history_uuid_of(const std::string& src)
{
    return make_uuid_from_string<HistoryUuidOf<o>>(src);
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

namespace Util {
namespace StrongType {

template <>
std::string to_string<boost::uuids::uuid>(const boost::uuids::uuid& src);

}//namespace Util::StrongType
}//namespace Util

#endif//UUID_HH_2F17826CB9BA26088AEC1F2ED71B2DD9
