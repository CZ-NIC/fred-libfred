/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
#include "libfred/registrar/certification/exceptions.hh"
#include "libfred/registrar/certification/get_registrar_certifications.hh"
#include "libfred/registrar/certification/registrar_certification_type.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {
namespace Registrar {

GetRegistrarCertifications::GetRegistrarCertifications(unsigned long long _registrar_id)
    : registrar_id_(_registrar_id)
{
}

std::vector<RegistrarCertification> GetRegistrarCertifications::exec(OperationContext& _ctx) const
{
    try
    {
        std::vector<RegistrarCertification> result;

        const Database::Result reg_exists = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT id FROM registrar WHERE id = $1::integer",
                // clang-format on
                Database::query_param_list(registrar_id_));
        if (reg_exists.size() != 1)
        {
            throw RegistrarNotFound();
        }

        const Database::Result certifications = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT id, valid_from, valid_until, classification, eval_file_id "
                "FROM registrar_certification WHERE registrar_id=$1::bigint "
                "ORDER BY valid_from DESC, id DESC",
                // clang-format on
                Database::query_param_list(registrar_id_));
        result.reserve(certifications.size());
        for (Database::Result::Iterator it = certifications.begin(); it != certifications.end(); ++it)
        {
            Database::Row::Iterator col = (*it).begin();
            RegistrarCertification rc;
            rc.id = *col;
            rc.valid_from = *(++col);
            rc.valid_until = *(++col);
            rc.classification = *(++col);
            rc.eval_file_id = *(++col);
            result.push_back(std::move(rc));
        }
        return result;
    }
    catch (const std::exception& e)
    {
        LOGGER.info(e.what());
        throw;
    }
    catch (...)
    {
        LOGGER.info("Failed to get registrar certifications due to unknown exception");
        throw;
    }
}

} // namespace LibFred::Registrar
} // namespace LibFred
