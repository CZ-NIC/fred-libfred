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
#include "libfred/registrar/epp_auth/delete_registrar_epp_auth.hh"
#include "libfred/registrar/epp_auth/exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <stdexcept>
#include <utility>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

DeleteRegistrarEppAuth::DeleteRegistrarEppAuth(EppAuthRecordCommonId _id)
    : id_{std::move(_id)}
{ }

namespace {

class Exec : public boost::static_visitor<Database::Result>
{
public:
    explicit Exec(const OperationContext& ctx)
        : ctx_{ctx}
    { }
    Database::Result operator()(unsigned long long id) const
    {
        return ctx_.get_conn().exec_params(
                // clang-format off
                "DELETE FROM registraracl "
                      "WHERE id = $1::BIGINT "
                  "RETURNING id, "
                            "uuid, "
                            "create_time, "
                            "cert, "
                            "password, "
                            "cert_data_pem",
                // clang-format on
                Database::QueryParams{id});
    }
    Database::Result operator()(const EppAuthRecordUuid& uuid) const
    {
        return ctx_.get_conn().exec_params(
                // clang-format off
                "DELETE FROM registraracl "
                      "WHERE uuid = $1::UUID "
                  "RETURNING id, "
                            "uuid, "
                            "create_time, "
                            "cert, "
                            "password, "
                            "cert_data_pem",
                // clang-format on
                Database::QueryParams{uuid.value});
    }
private:
    const OperationContext& ctx_;
};

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

}//namespace LibFred::Registrar::EppAuth::{anonymous}

unsigned long long DeleteRegistrarEppAuth::exec(const OperationContext& _ctx) const
{
    return delete_registrar_epp_auth(_ctx, id_).id;
}

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar::EppAuth;

EppAuthRecord LibFred::Registrar::EppAuth::delete_registrar_epp_auth(
        const OperationContext& ctx,
        const DeleteRegistrarEppAuth::EppAuthRecordCommonId& id)
{
    try
    {
        const auto db_result = boost::apply_visitor(Exec{ctx}, id);

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
        throw std::runtime_error("Duplicity in database");
    }
    catch (const NonexistentRegistrarEppAuth&)
    {
        throw;
    }
    catch (const Database::ResultFailed& e)
    {
        FREDLOG_INFO(boost::format{"Exception Database::ResultFailed caught: %1%"} % e.what());
    }
    catch (const std::exception& e)
    {
        FREDLOG_INFO(boost::format{"std::exception caught: %1%"} % e.what());
    }
    catch (...)
    {
        FREDLOG_INFO("Unknown exception caught");
    }
    throw DeleteRegistrarEppAuthException();
}
