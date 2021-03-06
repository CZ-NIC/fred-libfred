/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

namespace
{

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
        :id_(_id)
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
    const bool values_for_update_are_set = (from_date_ != boost::none || to_date_ != boost::none);
    if (!values_for_update_are_set)
    {
        throw NoUpdateData();
    }

    Database::QueryParams params;
    std::ostringstream object_sql;
    Util::HeadSeparator set_separator(" SET ", ", ");

    object_sql << "UPDATE registrarinvoice";
    if (from_date_ != boost::none)
    {
        if (!from_date_->is_special())
        {
            params.push_back(from_date_.get());
            object_sql << set_separator.get() << "fromdate = $" << params.size() << psql_type(from_date_);
        }
    }
    if (to_date_ != boost::none)
    {
        if (!to_date_->is_special())
        {
            params.push_back(to_date_.get());
            object_sql << set_separator.get() << "todate = $" << params.size() << psql_type(to_date_);
        }
    }

    params.push_back(id_);
    object_sql << " WHERE id = $" << params.size() << psql_type(id_) << " RETURNING id";

    try
    {
        const Database::Result update_result = _ctx.get_conn().exec_params(
                object_sql.str(),
                params);
        if (update_result.size() == 1)
        {
            const auto id = static_cast<unsigned long long>(update_result[0][0]);
            return id;
        }
        else if (update_result.size() < 1)
        {
            throw NonexistentZoneAccess();
        }
        else
        {
            throw std::runtime_error("Duplicity in database");
        }
    }
    catch (const NonexistentZoneAccess&)
    {
        throw;
    }
    catch (const std::exception&)
    {
        throw UpdateRegistrarZoneAccessException();
    }
}

} // namespace LibFred::Registrar::ZoneAccess
} // namespace LibFred::Registrar
} // namespace LibFred
