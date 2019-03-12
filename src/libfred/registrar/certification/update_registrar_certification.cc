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

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {
namespace Registrar {

UpdateRegistrarCertification::UpdateRegistrarCertification(
        unsigned long long _certification_id,
        boost::gregorian::date _valid_until)
    : certification_id_(_certification_id),
      valid_until_(_valid_until)
{
}

UpdateRegistrarCertification::UpdateRegistrarCertification(
        unsigned long long _certification_id,
        int _classification,
        unsigned long long _eval_file_id)
    : certification_id_(_certification_id),
      classification_(_classification),
      eval_file_id_(_eval_file_id)
{
}

void UpdateRegistrarCertification::exec(OperationContext& _ctx) const
{
    try
    {
        Database::ParamQuery query = Database::ParamQuery("UPDATE registrar_certification SET ");
        if (valid_until_ != boost::none)
        {
            if (valid_until_->is_special())
            {
                throw InvalidDateTo();
            }
            const Database::Result cert_in_past = _ctx.get_conn().exec_params(
                    // clang-format off
                    "SELECT now()::date > $1::date",
                    // clang-format on
                    Database::query_param_list(*valid_until_));
            const bool expired_date_to = static_cast<bool>(cert_in_past[0][0]);
            if (expired_date_to)
            {
                throw CertificationInPast();
            }

            const Database::Result from_until = _ctx.get_conn().exec_params(
                    // clang-format off
                    "SELECT valid_from FROM registrar_certification "
                    "WHERE id = $1::bigint FOR UPDATE",
                    // clang-format on
                    Database::query_param_list(certification_id_));
            const boost::gregorian::date old_from =
                    boost::gregorian::from_string(static_cast<std::string>(from_until[0][0]));
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
        LOGGER.info(e.what());
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
