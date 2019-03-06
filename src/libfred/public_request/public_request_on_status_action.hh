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
#ifndef PUBLIC_REQUEST_ON_STATUS_ACTION_HH_E316E8E231BF4548B9F2D7E1038A5A8E
#define PUBLIC_REQUEST_ON_STATUS_ACTION_HH_E316E8E231BF4548B9F2D7E1038A5A8E

#include "util/enum_conversion.hh"

namespace LibFred {
namespace PublicRequest {

struct OnStatusAction
{
    enum Enum
    {
        scheduled,
        processed,
        failed,
    };
};

}//Fred::PublicRequest
}//Fred

namespace Conversion {
namespace Enums {

inline std::string to_db_handle(LibFred::PublicRequest::OnStatusAction::Enum value)
{
    switch (value)
    {
        case LibFred::PublicRequest::OnStatusAction::scheduled: return "scheduled";
        case LibFred::PublicRequest::OnStatusAction::processed: return "processed";
        case LibFred::PublicRequest::OnStatusAction::failed:    return "failed";
    }
    throw std::invalid_argument("value doesn't exist in LibFred::PublicRequest::OnStatusAction::Enum");
}

template < >
inline LibFred::PublicRequest::OnStatusAction::Enum from_db_handle< LibFred::PublicRequest::OnStatusAction >(const std::string &db_handle)
{
    if (to_db_handle(LibFred::PublicRequest::OnStatusAction::scheduled) == db_handle) { return LibFred::PublicRequest::OnStatusAction::scheduled; }
    if (to_db_handle(LibFred::PublicRequest::OnStatusAction::processed) == db_handle) { return LibFred::PublicRequest::OnStatusAction::processed; }
    if (to_db_handle(LibFred::PublicRequest::OnStatusAction::failed) == db_handle) { return LibFred::PublicRequest::OnStatusAction::failed; }
    throw std::invalid_argument("handle \"" + db_handle + "\" isn't convertible to LibFred::PublicRequest::OnStatusAction::Enum");
}

} // namespace Conversion::Enums
} // namespace Conversion

#endif
