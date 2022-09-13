/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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

#ifndef REGISTRAR_EPP_AUTH_DATA_HH_6AAF7D673128405A9A77195271FC06E9
#define REGISTRAR_EPP_AUTH_DATA_HH_6AAF7D673128405A9A77195271FC06E9

#include <boost/optional.hpp>
#include <boost/uuid/uuid.hpp>

#include <chrono>
#include <initializer_list>
#include <set>
#include <string>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

struct EppAuthRecordUuid
{
    boost::uuids::uuid value;
};

struct EppAuthRecord
{
    using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
    unsigned long long id;
    EppAuthRecordUuid uuid;
    boost::optional<TimePoint> create_time;
    std::string certificate_fingerprint;
    std::string hashed_password;
    std::string cert_data_pem;
    friend bool operator<(const EppAuthRecord& lhs, const EppAuthRecord& rhs) noexcept
    {
        return lhs.id < rhs.id;
    }
};

struct RegistrarEppAuthData
{
    std::string registrar_handle;
    std::set<EppAuthRecord> epp_auth_records;
};

constexpr std::initializer_list<char> no_cert_data = {};

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
