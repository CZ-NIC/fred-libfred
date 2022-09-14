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

#include "libfred/db_settings.hh"
#include "libfred/registrar/epp_auth/exceptions.hh"
#include "libfred/registrar/epp_auth/get_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utility>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

}//namespace LibFred::Registrar::EppAuth::{anonymous}

GetRegistrarEppAuth::GetRegistrarEppAuth(std::string _registrar_handle)
    : registrar_handle_{std::move(_registrar_handle)}
{
}

RegistrarEppAuthData GetRegistrarEppAuth::exec(const OperationContext& _ctx) const
{
    try
    {
        RegistrarEppAuthData registrar_epp_auth_data;
        const Database::Result db_result = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT r.handle, "
                       "ra.id, "
                       "ra.uuid, "
                       "ra.create_time, "
                       "ra.cert, "
                       "ra.password, "
                       "ra.cert_data_pem "
                  "FROM registrar r "
                  "JOIN registraracl ra ON r.id = ra.registrarid "
                 "WHERE r.handle = UPPER($1::TEXT)",
                // clang-format on
                Database::query_param_list(registrar_handle_));

        if (db_result.size() > 0)
        {
            registrar_epp_auth_data.registrar_handle = static_cast<std::string>(db_result[0]["handle"]);
            for (std::size_t idx = 0; idx < db_result.size(); ++idx)
            {
                auto record = EppAuthRecord{
                        static_cast<unsigned long long>(db_result[idx]["id"]),
                        {to_uuid(db_result[idx]["uuid"])},
                        [](auto&& col_create_time) -> boost::optional<EppAuthRecord::TimePoint>
                        {
                            if (!col_create_time.isnull())
                            {
                                return static_cast<EppAuthRecord::TimePoint>(col_create_time);
                            }
                            return boost::none;
                        }(db_result[idx]["create_time"]),
                        static_cast<std::string>(db_result[idx]["cert"]),
                        static_cast<std::string>(db_result[idx]["password"]),
                        [](auto&& col_cert_data_pem)
                        {
                            if (!col_cert_data_pem.isnull())
                            {
                                return static_cast<std::string>(col_cert_data_pem);
                            }
                            return std::string{};
                        }(db_result[idx]["cert_data_pem"])};
                registrar_epp_auth_data.epp_auth_records.insert(std::move(record));
            }
        }
        return registrar_epp_auth_data;
    }
    catch (const std::exception&)
    {
        throw GetRegistrarEppAuthException();
    }
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred
