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
/**
 *  @file
 *  object id and handle pair
 */

#include "util/printable.hh"

#include "libfred/object/object_id_handle_pair.hh"

#include <boost/algorithm/string/case_conv.hpp>

#include <utility>

namespace LibFred {

ObjectIdHandlePair::ObjectIdHandlePair(const unsigned long long _id, const std::string& _handle)
    : id(_id), handle(_handle)
{
}

ObjectIdHandlePair::ObjectIdHandlePair()
    : id()
{
}

bool ObjectIdHandlePair::operator==(const ObjectIdHandlePair& _rhs) const
{
    return ((id == _rhs.id) &&
            (boost::algorithm::to_upper_copy(handle).compare(boost::algorithm::to_upper_copy(_rhs.handle)) == 0));
}

bool ObjectIdHandlePair::operator!=(const ObjectIdHandlePair& _rhs) const
{
    return !(*this == _rhs);
}

bool ObjectIdHandlePair::operator<(const ObjectIdHandlePair& _rhs) const
{
    return std::make_pair(id, boost::algorithm::to_upper_copy(handle)) <
           std::make_pair(_rhs.id, boost::algorithm::to_upper_copy(_rhs.handle));
}

std::string ObjectIdHandlePair::to_string() const
{
    return Util::format_data_structure("ObjectIdHandlePair",
            Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("id", boost::lexical_cast<std::string>(id)))
                    (std::make_pair("handle", handle))
            );
};

} // namespace LibFred
