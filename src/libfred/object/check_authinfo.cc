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

#include "libfred/object/check_authinfo.hh"
#include "libfred/opexception.hh"

#include "util/log/log.hh"
#include "util/password_storage.hh"

#include <utility>

namespace LibFred {
namespace Object {

namespace {

void lock_for_share(const OperationContext& ctx, const ObjectId& object_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "SELECT EXISTS(SELECT 0 "
                            "FROM object_registry "
                           "WHERE id = $1::INT AND "
                                 "erdate IS NULL "
                             "FOR SHARE)",
            Database::QueryParams{object_id});
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
}

auto visit_authinfos(
        const OperationContext& ctx,
        const ObjectId& object_id,
        const std::string& plaintext_password,
        CheckAuthinfo::Visitor on_match)
{
    const auto dbres = ctx.get_conn().exec_params(
            "SELECT id, password, registrar_id, created_at, expires_at "
              "FROM object_authinfo "
             "WHERE object_id = $1::INT AND "
                   "canceled_at IS NULL AND "
                   "NOW() < expires_at AND "
                   "password <> '' "
               "FOR UPDATE",
            Database::QueryParams{object_id});
    int match_counter = 0;
    for (std::size_t idx = 0; idx < dbres.size(); ++idx)
    {
        const auto authinfo_id = static_cast<AuthinfoId>(dbres[idx][0]);
        const auto encrypted_password_data =
                PasswordStorage::PasswordData::construct_from(static_cast<std::string>(dbres[idx][1]));
        try
        {
            PasswordStorage::check_password(plaintext_password, encrypted_password_data);
            ++match_counter;
            on_match(ctx, authinfo_id);
        }
        catch (const PasswordStorage::IncorrectPassword&)
        {
            FREDLOG_DEBUG(boost::format{"Password does not match the Authinfo %1% for object %2%, registrar %3%, "
                                        "created at %4% and expires at %5%"} %
                          *authinfo_id %
                          *object_id %
                          static_cast<std::string>(dbres[idx][2]) %
                          static_cast<std::string>(dbres[idx][3]) %
                          static_cast<std::string>(dbres[idx][4]));
        }
    }
    return match_counter;
}

struct CheckAuthinfoFailure : LibFred::InternalError
{
    CheckAuthinfoFailure() : LibFred::InternalError{""} { }
    const char* what() const noexcept override { return "check authinfo failed"; }
};

}//namespace LibFred::Object::{anonymous}

CheckAuthinfo::CheckAuthinfo(ObjectId object_id)
    : object_id_{std::move(object_id)}
{ }

int CheckAuthinfo::exec(const OperationContext& ctx, const std::string& plaintext_password, Visitor on_match) const
{
    try
    {
        lock_for_share(ctx, object_id_);
        return visit_authinfos(ctx, object_id_, plaintext_password, on_match);
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
        FREDLOG_ERROR(boost::format{"Check authinfo for object %1% failed: %2%"} % *object_id_ % e.what());
    }
    catch (...)
    {
        FREDLOG_ERROR(boost::format{"Check authinfo for object %1% failed: unknown exception caught"} % *object_id_);
    }
    throw CheckAuthinfoFailure{};
}

void CheckAuthinfo::increment_usage(const OperationContext& ctx, const AuthinfoId& authinfo_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "UPDATE object_authinfo "
               "SET use_count = use_count + 1 "
             "WHERE id = $1::INT AND "
                   "canceled_at IS NULL AND "
                   "NOW() < expires_at AND "
                   "password <> '' "
         "RETURNING object_id, registrar_id, created_at, expires_at, use_count",
            Database::QueryParams{authinfo_id});
    if (dbres.size() == 1)
    {
        FREDLOG_DEBUG(boost::format{"Authinfo %1% for object %2%, registrar %3%, created at %4% and expires at %5% "
                                    "was used %6% times" } %
                      *authinfo_id %
                      static_cast<std::string>(dbres[0][0]) %
                      static_cast<std::string>(dbres[0][1]) %
                      static_cast<std::string>(dbres[0][2]) %
                      static_cast<std::string>(dbres[0][3]) %
                      static_cast<std::string>(dbres[0][4]));
        return;
    }
    FREDLOG_WARNING(boost::format{"Unable to increment usage counter: authinfo %1% not found" } % *authinfo_id);
}

void CheckAuthinfo::increment_usage_and_cancel(const OperationContext& ctx, const AuthinfoId& authinfo_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "UPDATE object_authinfo "
               "SET canceled_at = NOW(), "
                   "password = NULL, "
                   "use_count = use_count + 1 "
             "WHERE id = $1::INT AND "
                   "canceled_at IS NULL AND "
                   "NOW() < expires_at AND "
                   "password <> '' "
         "RETURNING object_id, registrar_id, created_at, canceled_at, expires_at, use_count",
            Database::QueryParams{authinfo_id});
    if (dbres.size() == 1)
    {
        FREDLOG_DEBUG(boost::format{"Authinfo %1% for object %2%, registrar %3%, created at %4% and expires at %5% "
                                    "was used %6% times and canceled at %7%" } %
                      *authinfo_id %
                      static_cast<std::string>(dbres[0][0]) %
                      static_cast<std::string>(dbres[0][1]) %
                      static_cast<std::string>(dbres[0][2]) %
                      static_cast<std::string>(dbres[0][4]) %
                      static_cast<std::string>(dbres[0][5]) %
                      static_cast<std::string>(dbres[0][3]));
        return;
    }
    FREDLOG_ERROR(boost::format{"Unable to cancel and to increment usage counter: authinfo %1% not found" } % *authinfo_id);
    throw CheckAuthinfoFailure{};
}

std::string CheckAuthinfo::to_string() const
{
    std::string out;
    out =
        "{"
            "object_id:" + std::to_string(*object_id_) +
        "}";
    return out;
}

}//namespace LibFred::Object
}//namespace LibFred
