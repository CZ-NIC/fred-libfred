/*
 * Copyright (C) 2019  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libfred/registrable_object/registrable_object_reference.hh"

#include "util/printable.hh"

#include <boost/algorithm/string/case_conv.hpp>

#include <string>
#include <tuple>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum object_type>
RegistrableObjectReference<object_type>::RegistrableObjectReference(
        const unsigned long long _id,
        const std::string& _handle,
        const RegistrableObject::UuidOf<object_type>& _uuid)
    : id(_id),
      handle(_handle),
      uuid(_uuid)
{
}

template <Object_Type::Enum object_type>
RegistrableObjectReference<object_type>::RegistrableObjectReference()
    : id()
{
}

template <Object_Type::Enum object_type>
bool RegistrableObjectReference<object_type>::operator==(const RegistrableObjectReference& _rhs) const
{
    return id == _rhs.id &&
           boost::algorithm::to_upper_copy(handle).compare(boost::algorithm::to_upper_copy(_rhs.handle)) == 0 &&
           uuid == _rhs.uuid;
}

template <Object_Type::Enum object_type>
bool RegistrableObjectReference<object_type>::operator!=(const RegistrableObjectReference& _rhs) const
{
    return !(*this == _rhs);
}

template <Object_Type::Enum object_type>
bool RegistrableObjectReference<object_type>::operator<(const RegistrableObjectReference& _rhs) const
{
    return std::tie(id, boost::algorithm::to_upper_copy(handle), uuid) <
           std::tie(_rhs.id, boost::algorithm::to_upper_copy(_rhs.handle), _rhs.uuid);
}

template <Object_Type::Enum object_type>
std::string RegistrableObjectReference<object_type>::to_string() const
{
    return
            Util::format_data_structure(
                    "RegistrableObjectReference",
                    Util::vector_of<std::pair<std::string, std::string>>
                            (std::make_pair("id", boost::lexical_cast<std::string>(id)))
                            (std::make_pair("handle", handle))
                            (std::make_pair("uuid", uuid)));
};

} // namespace LibFred::Object
} // namespace LibFred
