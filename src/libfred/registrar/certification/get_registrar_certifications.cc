/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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

#include "libfred/registrar/certification/get_registrar_certifications.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace LibFred {
namespace Registrar {

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

}//namespace LibFred::Registrar::{anonymous}

GetRegistrarCertifications::GetRegistrarCertifications(unsigned long long _registrar_id)
    : registrar_id_(_registrar_id)
{ }

std::vector<RegistrarCertification> GetRegistrarCertifications::exec(const OperationContext& _ctx) const
{
    try
    {
        const auto dbres = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT rc.id, "
                       "rc.uuid, "
                       "rc.valid_from, "
                       "rc.valid_until, "
                       "rc.classification, "
                       "rc.eval_file_id, "
                       "rc.eval_file_uuid "
                  "FROM registrar r "
             "LEFT JOIN registrar_certification rc ON rc.registrar_id = r.id "
                 "WHERE r.id = $1::BIGINT "
              "ORDER BY 3 DESC, 1 DESC",
                // clang-format on
                Database::query_param_list(registrar_id_));
        if (dbres.size() == 0)
        {
            throw RegistrarNotFound{};
        }
        std::vector<RegistrarCertification> result;
        if (dbres[0][0].isnull())
        {
            return result;
        }
        result.reserve(dbres.size());
        for (unsigned idx = 0; idx < dbres.size(); ++idx)
        {
            const auto row = dbres[idx];
            RegistrarCertification rc;
            rc.id = static_cast<unsigned long long>(row[0]);
            rc.uuid = {to_uuid(row[1])};
            rc.valid_from = boost::gregorian::from_string(static_cast<std::string>(row[2]));
            rc.valid_until = row[3].isnull() ? boost::gregorian::date{boost::date_time::special_values::pos_infin}
                                             : boost::gregorian::from_string(static_cast<std::string>(row[3]));
            rc.classification = static_cast<int>(row[4]);
            rc.eval_file_id = static_cast<unsigned long long>(row[5]);
            rc.eval_file_uuid = {to_uuid(row[6])};
            result.push_back(std::move(rc));
        }
        return result;
    }
    catch (const std::exception& e)
    {
        FREDLOG_INFO(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to get registrar certifications due to unknown exception");
        throw;
    }
}

} // namespace LibFred::Registrar
} // namespace LibFred
