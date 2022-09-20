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
#include "libfred/registrar/epp_auth/add_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/exceptions.hh"

#include "util/log/logger.hh"
#include "util/password_storage.hh"

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

AddRegistrarEppAuth::AddRegistrarEppAuth(
            std::string _registrar_handle,
            std::string _certificate_fingerprint,
            std::string _plain_password,
            std::string _cert_data_pem)
    : registrar_handle_{std::move(_registrar_handle)},
      certificate_fingerprint_{std::move(_certificate_fingerprint)},
      plain_password_{std::move(_plain_password)},
      cert_data_pem_{std::move(_cert_data_pem)}
{
}

unsigned long long AddRegistrarEppAuth::exec(OperationContext& _ctx) const
{
    return add_registrar_epp_auth(
            _ctx,
            registrar_handle_,
            certificate_fingerprint_,
            plain_password_,
            cert_data_pem_).id;
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar::EppAuth;

EppAuthRecord LibFred::Registrar::EppAuth::add_registrar_epp_auth(
        OperationContext& ctx,
        const std::string& registrar_handle,
        const std::string& certificate_fingerprint,
        const std::string& plain_password,
        const std::string& cert_data_pem)
{
    const auto encrypted_password = PasswordStorage::encrypt_password_by_preferred_method(plain_password);

    try
    {
        const Database::Result db_result = ctx.get_conn().exec_params(
                // clang-format off
                "INSERT INTO registraracl (registrarid, cert, password, cert_data_pem) "
                     "SELECT r.id, $1::TEXT, $2::TEXT, $3::TEXT "
                       "FROM registrar AS r "
                      "WHERE r.handle = UPPER($4::TEXT) "
                  "RETURNING id, "
                            "uuid, "
                            "create_time, "
                            "cert, "
                            "password, "
                            "cert_data_pem",
                // clang-format on
                Database::QueryParams{
                        certificate_fingerprint,
                        encrypted_password.get_value(),
                        cert_data_pem.empty() ? Database::QueryParam{}
                                              : Database::QueryParam{cert_data_pem},
                        registrar_handle});

        if (db_result.size() == 1)
        {
            return EppAuthRecord{
                    static_cast<unsigned long long>(db_result[0]["id"]),
                    {to_uuid(db_result[0]["uuid"])},
                    [](auto&& col_create_time) -> boost::optional<EppAuthRecord::TimePoint>
                    {
                        if (!col_create_time.isnull())
                        {
                            return static_cast<EppAuthRecord::TimePoint>(col_create_time);
                        }
                        return boost::none;
                    }(db_result[0]["create_time"]),
                    static_cast<std::string>(db_result[0]["cert"]),
                    static_cast<std::string>(db_result[0]["password"]),
                    [](auto&& col_cert_data_pem)
                    {
                        if (!col_cert_data_pem.isnull())
                        {
                            return static_cast<std::string>(col_cert_data_pem);
                        }
                        return std::string{};
                    }(db_result[0]["cert_data_pem"])};
        }
        if (db_result.size() == 0)
        {
            throw NonexistentRegistrar();
        }
    }
    catch (const NonexistentRegistrar&)
    {
        throw;
    }
    catch (const Database::ResultFailed& e)
    {
        LOGGER.info(boost::format{"Exception Database::ResultFailed caught: %1%"} % e.what());
        throw DuplicateCertificate();
    }
    catch (const std::exception& e)
    {
        LOGGER.info(boost::format{"std::exception caught: %1%"} % e.what());
    }
    catch (...)
    {
        LOGGER.info("Unknown exception caught");
    }
    throw AddRegistrarEppAuthException();
}
