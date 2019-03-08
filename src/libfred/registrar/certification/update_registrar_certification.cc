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
#include "libfred/registrar/certification/update_registrar_certification.hh"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace LibFred {
namespace Registrar {

void UpdateRegistrarCertification::exec(OperationContext& _ctx)
{
    try
    {
        Database::ParamQuery query = Database::ParamQuery("UPDATE registrar_certification SET ");
        if (valid_until_)
        {
            const Database::Result cert_in_past = _ctx.get_conn().exec_params(
                    "SELECT now()::date > $1::date",
                    Database::query_param_list(*valid_until_));
            if (cert_in_past[0][0])
            {
                throw CertificationInPast();
            }

            const Database::Result from_until = _ctx.get_conn().exec_params(
                    "SELECT valid_from, valid_until FROM registrar_certification "
                    "WHERE id = $1::bigint FOR UPDATE",
                    Database::query_param_list(certification_id_));
            const boost::gregorian::date old_from = from_until[0][0];
            const boost::gregorian::date old_until = from_until[0][1];
            if (old_until < *valid_until_)
            {
                throw CertificationExtension();
            }
            if (old_from > *valid_until_)
            {
                throw WrongIntervalOrder();
            }

            query = query("valid_until = ")
                .param_date(*valid_until_);
        }
        else
        {
            constexpr int min_classification_value = 0;
            constexpr int max_classification_value = 5;

            const bool is_clasification_out_of_range = (*classification_ < min_classification_value) ||
                                                       (*classification_ > max_classification_value);
            if (is_clasification_out_of_range)
            {
                throw ScoreOutOfRange();
            }
            query = query("classification = ")
                            .param(*classification_, "integer")(", eval_file_id = ")
                            .param_bigint(*eval_file_id_);
        }
        query = query(" WHERE id = ")
                        .param_bigint(certification_id_);

        _ctx.get_conn().exec_params(query);
    }
    catch (const std::exception& e)
    {
        LOGGER.error(e.what());
        throw;
    }
    catch (...)
    {
        LOGGER.info("Failed to update registrar certification due to an unknown exception");
        throw;
    }
}

} // namespace LibFred::Registrar
} // namespace LibFred
