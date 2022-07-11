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

#include "libfred/object/clean_authinfo.hh"
#include "libfred/opexception.hh"

#include "util/log/log.hh"


namespace LibFred {
namespace Object {

namespace {

auto clean_authinfo(const OperationContext& ctx, const ObjectId& object_id)
{
    const auto dbres = ctx.get_conn().exec_params(
            "UPDATE object_authinfo "
               "SET password = NULL, "
                   "canceled_at = NOW() "
             "WHERE canceled_at IS NULL AND "
                   "password <> '' AND "
                   "NOW() < expires_at AND "
                   "object_id = $1::INT",
            Database::QueryParams{*object_id});
    return dbres.rows_affected();
}

}//namespace LibFred::Object::{anonymous}

CleanAuthinfo::CleanAuthinfo(ObjectId object_id)
    : object_id_{std::move(object_id)}
{ }

int CleanAuthinfo::exec(const OperationContext& ctx) const
{
    try
    {
        auto number_of_cleaned_authinfos = clean_authinfo(ctx, object_id_);
        LOGGER.debug(boost::format{"%1% authinfos was cleaned"} % number_of_cleaned_authinfos);
        return number_of_cleaned_authinfos;
    }
    catch (const std::exception& e)
    {
        LOGGER.error(boost::format{"Cleaning authinfo of object %1% failed: %2%"} % *object_id_ % e.what());
    }
    catch (...)
    {
        LOGGER.error(boost::format{"Cleaning authinfo of object %1% failed: unspecified exception caught"} % *object_id_);
    }
    struct InternalError : LibFred::InternalError
    {
        InternalError() : LibFred::InternalError{""} { }
        const char* what() const noexcept override { return "unable to clean authinfo of given object"; }
    };
    throw InternalError{};
}

std::string CleanAuthinfo::to_string() const
{
    return "{" + std::to_string(*object_id_) + "}";
}

}//namespace LibFred::Object
}//namespace LibFred
