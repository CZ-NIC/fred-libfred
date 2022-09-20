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
#include "libfred/registrar/epp_auth/clone_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/exceptions.hh"
#include "src/util/db/db_exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utility>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

CloneRegistrarEppAuth::CloneRegistrarEppAuth(
        EppAuthRecordCommonId id,
        std::string certificate_fingerprint,
        std::string certificate_data_pem)
    : id_{std::move(id)},
      fingerprint_{std::move(certificate_fingerprint)},
      data_{std::move(certificate_data_pem)}
{ }

namespace {

class Exec : public boost::static_visitor<Database::Result>
{
public:
    explicit Exec(OperationContext& ctx, const std::string& fingerprint, const std::string& data)
        : ctx_{ctx},
          fingerprint_{fingerprint},
          data_{data}
    { }
    Database::Result operator()(unsigned long long id) const
    {
        return ctx_.get_conn().exec_params(
                // clang-format off
                "INSERT INTO registraracl (registrarid, cert, password, cert_data_pem) "
                     "SELECT ra.registrarid, $1::TEXT, ra.password, $2::TEXT "
                       "FROM registraracl ra "
                      "WHERE ra.id = $3::BIGINT "
                  "RETURNING id, "
                            "uuid, "
                            "create_time, "
                            "cert, "
                            "password, "
                            "cert_data_pem",
                // clang-format on
                Database::QueryParams{
                        fingerprint_,
                        data_.empty() ? Database::QueryParam{}
                                      : Database::QueryParam{data_},
                        id});
    }
    Database::Result operator()(const EppAuthRecordUuid& uuid) const
    {
        return ctx_.get_conn().exec_params(
                // clang-format off
                "INSERT INTO registraracl (registrarid, cert, password, cert_data_pem) "
                     "SELECT ra.registrarid, $1::TEXT, ra.password, $2::TEXT "
                       "FROM registraracl ra "
                      "WHERE ra.uuid = $3::UUID "
                  "RETURNING id, "
                            "uuid, "
                            "create_time, "
                            "cert, "
                            "password, "
                            "cert_data_pem",
                // clang-format on
                Database::QueryParams{
                        fingerprint_,
                        data_.empty() ? Database::QueryParam{}
                                      : Database::QueryParam{data_},
                        uuid.value});
    }
private:
    OperationContext& ctx_;
    const std::string& fingerprint_;
    const std::string& data_;
};

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

}//namespace LibFred::Registrar::EppAuth::{anonymous}

unsigned long long CloneRegistrarEppAuth::exec(OperationContext& _ctx) const
{
    return clone_registrar_epp_auth(_ctx, id_, fingerprint_, data_).id;
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar::EppAuth;

EppAuthRecord LibFred::Registrar::EppAuth::clone_registrar_epp_auth(
        OperationContext& ctx,
        const CloneRegistrarEppAuth::EppAuthRecordCommonId& id,
        const std::string& certificate_fingerprint,
        const std::string& certificate_data_pem)
{
    try
    {
        const auto db_result = boost::apply_visitor(Exec{ctx, certificate_fingerprint, certificate_data_pem}, id);

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
            throw NonexistentRegistrarEppAuth();
        }
    }
    catch (const NonexistentRegistrarEppAuth&)
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
    throw CloneRegistrarEppAuthException();
}
