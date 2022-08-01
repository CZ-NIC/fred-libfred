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

#include "libfred/registrar/zone_access/delete_registrar_zone_access.hh"
#include "libfred/registrar/zone_access/exceptions.hh"

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

DeleteRegistrarZoneAccess::DeleteRegistrarZoneAccess(unsigned long long access_id)
    : access_id_{access_id}
{ }

unsigned long long DeleteRegistrarZoneAccess::exec(OperationContext& ctx) const
{
    try
    {
        const auto dbres = ctx.get_conn().exec_params(
                "DELETE FROM registrarinvoice "
                "WHERE id = $1::BIGINT "
                "RETURNING id",
                Database::QueryParams{access_id_});

        if (dbres.size() == 1)
        {
            return static_cast<unsigned long long>(dbres[0][0]);
        }
        if (dbres.size() <= 0)
        {
            struct ZoneAccessDoesNotExist : NonexistentZoneAccess, DeleteRegistrarZoneAccessException
            {
                const char* what() const noexcept override { return "Registrar zone access does not exist, can not be deleted"; }
            };
            throw ZoneAccessDoesNotExist{};
        }
        struct UnexpectedNumberOfRows : DeleteRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "At most one row to delete expected"; }
        };
        throw UnexpectedNumberOfRows{};
    }
    catch (const DeleteRegistrarZoneAccessException& e)
    {
        ctx.get_log().info(boost::format{"Exception caught by delete: %1%"} % e.what());
        throw;
    }
    catch (const std::exception& e)
    {
        ctx.get_log().info(boost::format{"Unexpected std::exception caught by delete: %1%"} % e.what());
        struct UnexpectedStdException : DeleteRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "Unexpected std::exception caught by registrar zone access delete"; }
        };
        throw UnexpectedStdException{};
    }
    catch (...)
    {
        ctx.get_log().info("Unexpected exception caught by delete");
        struct UnexpectedException : DeleteRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "Unexpected exception caught by registrar zone access delete"; }
        };
        throw UnexpectedException{};
    }
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

