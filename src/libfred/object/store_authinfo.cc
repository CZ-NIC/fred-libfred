/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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

#include "libfred/object/store_authinfo.hh"
#include "libfred/opexception.hh"

#include "util/log/log.hh"
#include "util/password_storage/impl/pbkdf2.hh"

#include <utility>

namespace LibFred {
namespace Object {

namespace {

auto get_valid_ttl(std::chrono::seconds ttl)
{
    static constexpr auto no_ttl = std::chrono::seconds{0};
    if (ttl <= no_ttl)
    {
        struct TtlTooShort : InvalidTtl
        {
            const char* what() const noexcept override { return "TTL must be greater than 0 sec"; }
        };
        throw TtlTooShort{};
    }
    return ttl;
}

void lock_for_share(
        const OperationContext& ctx,
        const ObjectId& object_id,
        unsigned long long registrar_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "SELECT EXISTS(SELECT 0 "
                            "FROM object_registry "
                           "WHERE id = $1::INT AND "
                                 "erdate IS NULL "
                             "FOR SHARE), "
                   "EXISTS(SELECT 0 "
                            "FROM registrar "
                           "WHERE id = $2::INT "
                             "FOR SHARE)",
            Database::QueryParams{object_id, registrar_id});
    if (dbres.size() != 1)
    {
        struct InvalidQuery : LibFred::InternalError
        {
            InvalidQuery()
                 : LibFred::InternalError{""}
            { }
            const char* what() const noexcept override { return "invalid query (must return just 1 row)"; }
        };
        throw InvalidQuery{};
    }
    const auto object_exists = static_cast<bool>(dbres[0][0]);
    if (!object_exists)
    {
        struct ObjectDoesNotExist : LibFred::UnknownObjectId
        {
            const char* what() const noexcept override { return "object_id does not refer any existing registrable object"; }
        };
        throw ObjectDoesNotExist{};
    }
    const auto registrar_exists = static_cast<bool>(dbres[0][1]);
    if (!registrar_exists)
    {
        struct RegistrarDoesNotExist : LibFred::UnknownRegistrar
        {
            const char* what() const noexcept override { return "registrar_id does not refer any existing registrar"; }
        };
        throw RegistrarDoesNotExist{};
    }
}

auto deactivate_password(
        const OperationContext& ctx,
        const ObjectId& object_id,
        unsigned long long registrar_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "UPDATE object_authinfo "
               "SET password = '' "
             "WHERE object_id = $1::INT AND "
                   "registrar_id = $2::INT AND "
                   "canceled_at IS NULL AND "
                   "password <> '' "
         "RETURNING id",
            Database::QueryParams{object_id, registrar_id});
    if (dbres.size() == 1)
    {
        FREDLOG_DEBUG("One active password deactivated");
        return static_cast<AuthinfoId>(dbres[0][0]);
    }
    if (dbres.size() != 0)
    {
        FREDLOG_ERROR(boost::format{"Too many active passwords for object %1% and registrar %2%"} % *object_id % registrar_id);
    }
    static constexpr auto invalid_object_authinfo_id = AuthinfoId{0ull};
    return invalid_object_authinfo_id;
}

auto encrypt_password(const std::string& plaintext_password)
{
    static constexpr auto hash_function = PasswordStorage::Impl::Pbkdf2::HashFunction::sha512;
    static constexpr auto number_of_iterations = 10;
    using Encryptor = PasswordStorage::Impl::AlgPbkdf2<hash_function, number_of_iterations>;
    return Encryptor::encrypt_password(plaintext_password);
}

struct StorePasswordFailure : LibFred::InternalError
{
    StorePasswordFailure() : LibFred::InternalError{""} { }
    const char* what() const noexcept override { return "unable to store password"; }
};

auto store_password(
        const OperationContext& ctx,
        const ObjectId& object_id,
        unsigned long long registrar_id,
        const std::chrono::seconds& ttl,
        const PasswordStorage::PasswordData& encrypted_password)
{
    const auto dbres = ctx.get_conn().exec_params(
            "INSERT INTO object_authinfo (object_id, registrar_id, password, expires_at) "
            "VALUES ($1::INT, $2::INT, $3::VARCHAR, NOW() + $4::INTERVAL) "
         "RETURNING id, created_at, expires_at",
            Database::QueryParams{
                    object_id,
                    registrar_id,
                    encrypted_password.get_value(),
                    std::to_string(ttl.count()) + "SECONDS"});
    if (dbres.size() == 1)
    {
        auto authinfo_id = static_cast<AuthinfoId>(dbres[0][0]);
        FREDLOG_DEBUG(boost::format{"Password for object %1% and registrar %2% stored with id %3% "
                                    "at %4% expires at %5%"} %
                      *object_id %
                      registrar_id %
                      *authinfo_id %
                      static_cast<std::string>(dbres[0][1]) %
                      static_cast<std::string>(dbres[0][2]));
        return authinfo_id;
    }
    FREDLOG_ERROR(boost::format{"Unable to store password for object %1% and registrar %2%"} % *object_id % registrar_id);
    throw StorePasswordFailure{};
}

}//namespace LibFred::Object::{anonymous}

StoreAuthinfo::StoreAuthinfo(
        ObjectId object_id,
        unsigned long long registrar_id,
        std::chrono::seconds ttl)
    : object_id_{std::move(object_id)},
      registrar_id_{registrar_id},
      ttl_{get_valid_ttl(ttl)}
{ }

AuthinfoId StoreAuthinfo::exec(const OperationContext& ctx, const std::string& plaintext_password) const
{
    try
    {
        lock_for_share(ctx, object_id_, registrar_id_);
        deactivate_password(ctx, object_id_, registrar_id_);
        return store_password(ctx, object_id_, registrar_id_, ttl_, encrypt_password(plaintext_password));
    }
    catch (const LibFred::UnknownRegistrar&)
    {
        throw;
    }
    catch (const LibFred::UnknownObjectId&)
    {
        throw;
    }
    catch (const LibFred::InternalError&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(boost::format{"Store password for object %1% and registrar %2% failed: %3%"} %
                                    *object_id_ %
                                    registrar_id_ %
                                    e.what());
    }
    catch (...)
    {
        FREDLOG_ERROR(boost::format{"Store password for object %1% and registrar %2% failed: unknown exception caught"} %
                                    *object_id_ %
                                    registrar_id_);
    }
    throw StorePasswordFailure{};
}

std::string StoreAuthinfo::to_string() const
{
    std::string out;
    out =
        "{"
            "object_id:" + std::to_string(*object_id_) + ", "
            "registrar_id:" + std::to_string(registrar_id_) + ", "
            "ttl:" + std::to_string(ttl_.count()) + " sec"
        "}";
    return out;
}

}//namespace LibFred::Object
}//namespace LibFred
