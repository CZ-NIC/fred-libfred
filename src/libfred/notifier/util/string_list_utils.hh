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
 */

#ifndef STRING_LIST_UTILS_HH_985C634B52D6452A998B887466E4AEA6
#define STRING_LIST_UTILS_HH_985C634B52D6452A998B887466E4AEA6

#include "libfred/registrable_object/registrable_object_reference.hh"

#include <boost/asio/ip/address.hpp>

#include <algorithm>
#include <string>
#include <vector>

namespace Notification {

    template <LibFred::Object_Type::Enum object_type>
    inline std::vector<std::string> get_handles(const std::vector<LibFred::RegistrableObject::RegistrableObjectReference<object_type>>& _in) {
        std::vector<std::string> result;
        result.reserve(_in.size());
        std::transform(
                _in.cbegin(),
                _in.cend(),
                std::back_inserter(result),
                [](const auto& o) { return o.handle; });
        return result;
    }

    inline std::vector<std::string> get_string_addresses(const std::vector<boost::asio::ip::address>& _in) {
        std::vector<std::string> result;
        result.reserve(_in.size());
        std::transform(
                _in.cbegin(),
                _in.cend(),
                std::back_inserter(result),
                [](const auto& o) { return o.to_string(); });
        return result;
    }

    /**
     * interface adapter for std::sort
     */
    inline std::vector<std::string> sort(const std::vector<std::string>& _in) {
        std::vector<std::string> result = _in;
        std::sort(result.begin(), result.end());

        return result;
    }
}

#endif
