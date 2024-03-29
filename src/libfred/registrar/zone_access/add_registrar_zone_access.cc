/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#include "libfred/opcontext.hh"
#include "libfred/registrar/zone_access/add_registrar_zone_access.hh"
#include "libfred/registrar/zone_access/exceptions.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <string>

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

AddRegistrarZoneAccess::AddRegistrarZoneAccess(
        const std::string& _registrar_handle,
        const std::string& _zone_fqdn,
        const boost::gregorian::date& _from_date)
        : registrar_handle_(_registrar_handle),
          zone_fqdn_(_zone_fqdn),
          from_date_(_from_date),
          to_date_(not_a_date_time)
{
}

AddRegistrarZoneAccess& AddRegistrarZoneAccess::set_to_date(
        const boost::optional<boost::gregorian::date>& _to_date)
{
    to_date_ = _to_date;
    return *this;
}

unsigned long long AddRegistrarZoneAccess::exec(const OperationContext& _ctx) const
{
    if (from_date_.is_special())
    {
        throw InvalidDateFrom();
    }

    try
    {
        Database::QueryParam to_date = Database::QPNull;

        if (to_date_ != boost::none)
        {
            if (!to_date_->is_special())
            {
                to_date = Database::QueryParam(to_date_.get());
            }
        }

        _ctx.get_conn().exec("LOCK TABLE registrarinvoice IN ACCESS EXCLUSIVE MODE");
        const Database::Result overlapping_zone_access = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT z.fqdn FROM registrarinvoice ri "
                "JOIN registrar r ON r.id = ri.registrarid "
                "JOIN zone z ON z.id = ri.zone "
                "WHERE r.handle = UPPER($1::text) "
                "AND z.fqdn = LOWER($2::text) "
                "AND ((ri.todate IS NULL OR ri.todate >= $3::date) "
                "AND ($4::date IS NULL OR ri.fromdate <= $4::date)) ",
                // clang-format on
                Database::query_param_list(registrar_handle_)
                                        (zone_fqdn_)
                                        (from_date_)
                                        (to_date));

        if (overlapping_zone_access.size() > 0)
        {
            throw OverlappingZoneAccessRange();
        }

        const Database::Result insert_result = _ctx.get_conn().exec_params(
                // clang-format off
                "INSERT INTO registrarinvoice (registrarid, zone, fromdate, todate) "
                "SELECT r.id, (SELECT z.id FROM zone AS z WHERE z.fqdn = LOWER($1::text)), $2::date, $3::date "
                "FROM registrar AS r WHERE r.handle = UPPER($4::text) "
                "RETURNING id",
                // clang-format on
                Database::query_param_list(zone_fqdn_)
                                          (from_date_)
                                          (to_date)
                                          (registrar_handle_));

        if (insert_result.size() == 1)
        {
            const auto id = static_cast<unsigned long long>(insert_result[0][0]);
            return id;
        }
        if (insert_result.size() == 0)
        {
            throw NonexistentRegistrar();
        }
        throw std::runtime_error("Duplicity in database");
    }
    catch (const OverlappingZoneAccessRange&)
    {
        throw;
    }
    catch (const NonexistentRegistrar&)
    {
        throw;
    }
    catch (const Database::ResultFailed&)
    {
        throw NonexistentZone();
    }
    catch (const std::exception&)
    {
        throw AddRegistrarZoneAccessException();
    }
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred

