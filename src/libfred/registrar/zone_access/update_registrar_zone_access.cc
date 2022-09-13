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

#include "libfred/registrar/zone_access/exceptions.hh"
#include "libfred/registrar/zone_access/update_registrar_zone_access.hh"
#include "src/util/db/query_param.hh"

#include <sstream>
#include <string>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

namespace {

constexpr const char * psql_type(const boost::optional<boost::gregorian::date>&)
{
    return "::date";
}

constexpr const char * psql_type(const unsigned long long&)
{
    return "::bigint";
}

} // namespace LibFred::Registrar::ZoneAccess::{anonymous}

UpdateRegistrarZoneAccess::UpdateRegistrarZoneAccess(const unsigned long long _id)
    : id_(_id)
{
}

UpdateRegistrarZoneAccess& UpdateRegistrarZoneAccess::set_from_date(
        const boost::optional<boost::gregorian::date>& _from_date)
{
    from_date_ = _from_date;
    return *this;
}

UpdateRegistrarZoneAccess& UpdateRegistrarZoneAccess::set_to_date(
        const boost::optional<boost::gregorian::date>& _to_date)
{
    to_date_ = _to_date;
    return *this;
}

unsigned long long UpdateRegistrarZoneAccess::exec(OperationContext& _ctx) const
{
    const bool values_for_update_are_set =
            (from_date_ != boost::none && !from_date_->is_special()) ||
            (to_date_ != boost::none && !to_date_->is_special());
    if (!values_for_update_are_set)
    {
        struct NothingToUpdate : NoUpdateData, UpdateRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "No data to update"; }
        };
        throw NothingToUpdate{};
    }

    Database::QueryParams params;
    std::ostringstream sql;
    Util::HeadSeparator delimiter("", ", ");

    sql << "UPDATE registrarinvoice lhs SET ";
    if (from_date_ != boost::none && !from_date_->is_special())
    {
        params.push_back(from_date_.get());
        sql << delimiter.get() << "fromdate = $" << params.size() << psql_type(from_date_);
    }
    if (to_date_ != boost::none && !to_date_->is_special())
    {
        params.push_back(to_date_.get());
        sql << delimiter.get() << "todate = $" << params.size() << psql_type(to_date_);
    }

    params.push_back(id_);
    sql << " "
        "WHERE id = $" << params.size() << psql_type(id_) << " "
    "RETURNING id, "
              "EXISTS(SELECT 0 "
                       "FROM registrarinvoice rhs "
                      "WHERE rhs.registrarid = lhs.registrarid AND "
                            "rhs.zone = lhs.zone AND "
                            "rhs.id != lhs.id AND "
                            "(lhs.fromdate <= rhs.todate OR rhs.todate IS NULL) AND "
                            "(rhs.fromdate <= lhs.todate OR lhs.todate IS NULL)), "
              "lhs.todate < lhs.fromdate";

    try
    {
        const auto dbres = _ctx.get_conn().exec_params(sql.str(), params);
        if (dbres.size() == 1)
        {
            const bool has_overlapped_periods = static_cast<bool>(dbres[0][1]);
            if (has_overlapped_periods)
            {
                struct OverlappedPeriods : OverlappingZoneAccessRange, UpdateRegistrarZoneAccessException
                {
                    const char* what() const noexcept override { return "Overlapped zone access periods after update"; }
                };
                throw OverlappedPeriods{};
            }
            const bool has_invalid_period =
                    !dbres[0][2].isnull() && static_cast<bool>(dbres[0][2]);
            if (has_invalid_period)
            {
                struct InvalidPeriod : InvalidZoneAccessPeriod, UpdateRegistrarZoneAccessException
                {
                    const char* what() const noexcept override { return "Invalid zone access period after update"; }
                };
                throw InvalidPeriod{};
            }
            const auto id = static_cast<unsigned long long>(dbres[0][0]);
            return id;
        }
        if (dbres.size() < 1)
        {
            struct ZoneAccessDoesNotExist : NonexistentZoneAccess, UpdateRegistrarZoneAccessException
            {
                const char* what() const noexcept override { return "Register zone access not found"; }
            };
            throw ZoneAccessDoesNotExist{};
        }
        struct UnexpectedNumberOfRows : UpdateRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "At most one row expected"; }
        };
        throw UnexpectedNumberOfRows{};
    }
    catch (const UpdateRegistrarZoneAccessException& e)
    {
        _ctx.get_log().info(boost::format{"Exception caught by update: %1%"} % e.what());
        throw;
    }
    catch (const std::exception& e)
    {
        _ctx.get_log().info(boost::format{"Unexpected std::exception caught by update: %1%"} % e.what());
        struct UnexpectedStdException : UpdateRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "Unexpected std::exception caught by update"; }
        };
        throw UnexpectedStdException{};
    }
    catch (...)
    {
        _ctx.get_log().info("Unexpected exception caught by update");
        struct UnexpectedException : UpdateRegistrarZoneAccessException
        {
            const char* what() const noexcept override { return "Unexpected exception caught by update"; }
        };
        throw UnexpectedException{};
    }
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred
