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

#include "libfred/object/clean_expired_authinfos.hh"
#include "libfred/opexception.hh"

#include "util/log/log.hh"


namespace LibFred {
namespace Object {

namespace {

int clean_expired_authinfos(const OperationContext& ctx)
{
    return ctx.get_conn().exec(
            "UPDATE object_authinfo "
               "SET password = NULL, "
                   "canceled_at = NOW() "
             "WHERE canceled_at IS NULL AND "
                   "password <> '' AND "
                   "expires_at <= NOW() "
         "RETURNING id").size();
}

}//namespace LibFred::Object::{anonymous}

int CleanExpiredAuthinfos::exec(const OperationContext& ctx) const
{
    try
    {
        auto number_of_cleaned_authinfos = clean_expired_authinfos(ctx);
        FREDLOG_DEBUG(boost::format{"%1% expired authinfos was cleaned"} % number_of_cleaned_authinfos);
        return number_of_cleaned_authinfos;
    }
    catch (const std::exception& e)
    {
        FREDLOG_ERROR(boost::format{"Cleaning expired authinfos failed: %1%"} % e.what());
    }
    catch (...)
    {
        FREDLOG_ERROR("Cleaning expired authinfos failed: unspecified exception caught");
    }
    struct InternalError : LibFred::InternalError
    {
        InternalError() : LibFred::InternalError{""} { }
        const char* what() const noexcept override { return "unable to clean expired authinfos"; }
    };
    throw InternalError{};
}

std::string CleanExpiredAuthinfos::to_string() const
{
    return "{}";
}

}//namespace LibFred::Object
}//namespace LibFred
