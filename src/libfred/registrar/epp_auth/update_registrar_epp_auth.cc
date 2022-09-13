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
#include "libfred/registrar/epp_auth/update_registrar_epp_auth.hh"

#include "src/util/db/query_param.hh"
#include "src/util/password_storage.hh"
#include "src/util/util.hh"

#include <stdexcept>
#include <utility>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

UpdateRegistrarEppAuth::UpdateRegistrarEppAuth(EppAuthRecordCommonId _id)
    : id_{std::move(_id)},
      certificate_fingerprint_{},
      plain_password_{},
      certificate_data_pem_{}
{ }

UpdateRegistrarEppAuth& UpdateRegistrarEppAuth::set_certificate_fingerprint(
        boost::optional<std::string> certificate_fingerprint)
{
    return this->set_certificate(std::move(certificate_fingerprint), std::string{});
}

UpdateRegistrarEppAuth& UpdateRegistrarEppAuth::set_certificate(
        boost::optional<std::string> _certificate_fingerprint,
        boost::optional<std::string> _certificate_data_pem)
{
    certificate_fingerprint_ = std::move(_certificate_fingerprint);
    certificate_data_pem_ = std::move(_certificate_data_pem);
    return *this;
}

UpdateRegistrarEppAuth& UpdateRegistrarEppAuth::set_plain_password(
        boost::optional<std::string> _plain_password)
{
    plain_password_ = std::move(_plain_password);
    return *this;
}

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

class Exec : public boost::static_visitor<Database::Result>
{
public:
    explicit Exec(
            OperationContext& ctx,
            const UpdateRegistrarEppAuth::EppAuthRecordCommonId& id,
            const boost::optional<std::string>& certificate_fingerprint,
            const boost::optional<std::string>& plain_password,
            const boost::optional<std::string>& certificate_data_pem)
        : ctx_{ctx},
          id_{id},
          certificate_fingerprint_{certificate_fingerprint},
          plain_password_{plain_password},
          certificate_data_pem_{certificate_data_pem}
    { }
    Database::Result operator()(unsigned long long id) const
    {
        auto params = Database::QueryParams{id};
        return ctx_.get_conn().exec_params(
                // clang-format off
                "UPDATE registraracl "
                   "SET " + this->make_set_part(params) + " "
                 "WHERE id = $1::BIGINT "
             "RETURNING id, "
                       "uuid, "
                       "create_time, "
                       "cert, "
                       "password, "
                       "cert_data_pem",
                // clang-format on
                params);
    }
    Database::Result operator()(const EppAuthRecordUuid& uuid) const
    {
        auto params = Database::QueryParams{uuid.value};
        return ctx_.get_conn().exec_params(
                // clang-format off
                "UPDATE registraracl "
                   "SET " + this->make_set_part(params) + " "
                 "WHERE uuid = $1::UUID "
             "RETURNING id, "
                       "uuid, "
                       "create_time, "
                       "cert, "
                       "password, "
                       "cert_data_pem",
                // clang-format on
                params);
    }
private:
    std::string make_set_part(Database::QueryParams& params) const
    {
        std::string result;
        const auto append = [&](auto&& str)
        {
            if (!result.empty())
            {
                result.append(", ");
            }
            result.append(str);
        };
        if (certificate_fingerprint_ != boost::none)
        {
            params.emplace_back(*certificate_fingerprint_);
            append("cert = $" + std::to_string(params.size()) + "::TEXT");
        }
        if (plain_password_ != boost::none)
        {
            params.emplace_back(
                    PasswordStorage::encrypt_password_by_preferred_method(*plain_password_).get_value());
            append("password = $" + std::to_string(params.size()) + "::TEXT");
        }
        if (certificate_data_pem_ != boost::none)
        {
            if (certificate_data_pem_->empty())
            {
                params.emplace_back(); // NULL
            }
            else
            {
                params.emplace_back(*certificate_data_pem_);
            }
            append("cert_data_pem = $" + std::to_string(params.size()) + "::TEXT");
        }
        return result;
    }
    OperationContext& ctx_;
    const UpdateRegistrarEppAuth::EppAuthRecordCommonId& id_;
    const boost::optional<std::string>& certificate_fingerprint_;
    const boost::optional<std::string>& plain_password_;
    const boost::optional<std::string>& certificate_data_pem_;
};

}//namespace LibFred::Registrar::EppAuth::{anonymous}

void UpdateRegistrarEppAuth::exec(OperationContext& _ctx) const
{
    update_registrar_epp_auth(_ctx, id_, certificate_fingerprint_, plain_password_, certificate_data_pem_);
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar::EppAuth;

EppAuthRecord LibFred::Registrar::EppAuth::update_registrar_epp_auth(
        OperationContext& ctx,
        const UpdateRegistrarEppAuth::EppAuthRecordCommonId& id,
        const boost::optional<std::string>& certificate_fingerprint,
        const boost::optional<std::string>& plain_password,
        const boost::optional<std::string>& certificate_data_pem)
{
    const bool values_for_update_are_set = (certificate_fingerprint != boost::none ||
                                            plain_password != boost::none ||
                                            certificate_data_pem != boost::none);

    if (!values_for_update_are_set)
    {
        throw NoUpdateData();
    }

    try
    {
        const auto db_result = boost::apply_visitor(
                Exec{ctx, id, certificate_fingerprint, plain_password, certificate_data_pem},
                id);

        if (db_result.size() == 1)
        {
            return EppAuthRecord{
                    static_cast<unsigned long long>(db_result[0]["id"]),
                    {to_uuid(db_result[0]["uuid"])},
                    static_cast<EppAuthRecord::TimePoint>(db_result[0]["create_time"]),
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
    throw UpdateRegistrarEppAuthException();
}
