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
 *  registrar group type
 */

#ifndef REGISTRAR_GROUP_TYPE_HH_DED9CE0DDAC34F88A10EBB82E2F7AA15
#define REGISTRAR_GROUP_TYPE_HH_DED9CE0DDAC34F88A10EBB82E2F7AA15

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>

namespace LibFred {
namespace Registrar {

struct RegistrarGroup
{
    unsigned long long id;
    std::string name;
    boost::posix_time::ptime cancelled;

    bool operator==(const RegistrarGroup& _other) const
    {
        return (id == _other.id &&
            name == _other.name &&
            cancelled == _other.cancelled);
    }
};

} // namespace Registrar
} // namespace LibFred

#endif
