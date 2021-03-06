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
*  header of LibFred::PublicRequest::Status class
*/
#ifndef PUBLIC_REQUEST_STATUS_HH_3BF6CC4D1E3A43799970F76F3DD62B96
#define PUBLIC_REQUEST_STATUS_HH_3BF6CC4D1E3A43799970F76F3DD62B96

#include "util/enum_conversion.hh"

/// Fred matters
namespace LibFred {
/// Fred public request matters
namespace PublicRequest {

/**
 * Bidirectional conversions between string and enum representation of public request status.
 */
struct Status
{
    /**
     * Names of particular public request status.
     */
    enum Enum//enum_public_request_status table
    {
        opened,
        resolved,
        invalidated,
    };
};

}//Fred::PublicRequest
}//Fred

namespace Conversion {
namespace Enums {

inline std::string to_db_handle(LibFred::PublicRequest::Status::Enum value)
{
    switch (value)
    {
        case LibFred::PublicRequest::Status::opened: return "opened";
        case LibFred::PublicRequest::Status::resolved: return "resolved";
        case LibFred::PublicRequest::Status::invalidated: return "invalidated";
    }
    throw std::invalid_argument("value doesn't exist in LibFred::PublicRequest::Status::Enum");
}

template < >
inline LibFred::PublicRequest::Status::Enum from_db_handle< LibFred::PublicRequest::Status >(const std::string &db_handle)
{
    if (to_db_handle(LibFred::PublicRequest::Status::opened) == db_handle) { return LibFred::PublicRequest::Status::opened; }
    if (to_db_handle(LibFred::PublicRequest::Status::resolved) == db_handle) { return LibFred::PublicRequest::Status::resolved; }
    if (to_db_handle(LibFred::PublicRequest::Status::invalidated) == db_handle) { return LibFred::PublicRequest::Status::invalidated; }
    throw std::invalid_argument("handle \"" + db_handle + "\" isn't convertible to LibFred::PublicRequest::Status::Enum");
}

} // namespace Conversion::Enums
} // namespace Conversion

#endif
