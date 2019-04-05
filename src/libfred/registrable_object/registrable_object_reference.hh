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

#ifndef OBJECT_REFERENCE_HH_6F08393EBC044C629F6CF8262D8E36CC
#define OBJECT_REFERENCE_HH_6F08393EBC044C629F6CF8262D8E36CC

#include "libfred/object/object_type.hh"
#include "libfred/registrable_object/uuid.hh"

#include "util/printable.hh"

#include <string>

namespace LibFred {
namespace RegistrableObject {

/**
 * RegistrableObject identification structure.
 */
template <Object_Type::Enum object_type>
struct RegistrableObjectReference : Util::Printable<RegistrableObjectReference<object_type>>
{
    /**
    * Default constructor of data structure.
    */
    RegistrableObjectReference();

    RegistrableObjectReference(const RegistrableObjectReference&) = default;
    RegistrableObjectReference(RegistrableObjectReference&&) = default;
    RegistrableObjectReference& operator=(const RegistrableObjectReference&) = default;
    RegistrableObjectReference& operator=(RegistrableObjectReference&&) = default;

    /**
    * Constructor of data structure.
    * @param id sets object database id into @ref id attribute
    * @param handle sets object handle into @ref handle attribute
    * @param uuid sets object uuid into @ref uuid attribute
    */
    RegistrableObjectReference(unsigned long long id, const std::string& handle, const RegistrableObject::UuidOf<object_type>& uuid);

    /**
     * Comparison operator comparing all attributes.
     * Handle is compared case insensitive
     * @param rhs data compared with this instance
     */
    bool operator==(const RegistrableObjectReference& rhs) const;

    /**
     * Comparison operator comparing all attributes.
     * Handle is compared case insensitive
     * @param rhs data compared with this instance
     */
    bool operator!=(const RegistrableObjectReference& rhs) const;

    /**
     * Comparison operator comparing all attributes
     * Handle is compared case insensitive
     * @param rhs data compared with this instance
     */
    bool operator<(const RegistrableObjectReference& rhs) const;

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;

    unsigned long long id; /**< database id of the object */
    std::string handle; /**< handle of the object */
    RegistrableObject::UuidOf<object_type> uuid; /**< UUID of the object */
};

} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
