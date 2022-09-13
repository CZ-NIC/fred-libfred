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

#ifndef UPDATE_REGISTRAR_CERTIFICATION_HH_4BE6201D6B1B476795A61FD9C4042041
#define UPDATE_REGISTRAR_CERTIFICATION_HH_4BE6201D6B1B476795A61FD9C4042041

#include "libfred/opcontext.hh"
#include "libfred/registrar/certification/registrar_certification_type.hh"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace LibFred {
namespace Registrar {

class UpdateRegistrarCertification
{
public:
    using RegistrarCertificationIdVariant = boost::variant<unsigned long long, RegistrarCertificationUuid>;
    using FileIdVariant = boost::variant<unsigned long long, FileUuid>;
    explicit UpdateRegistrarCertification(const RegistrarCertificationIdVariant& certification_id);
    explicit UpdateRegistrarCertification(
            const RegistrarCertificationIdVariant& certification_id,
            const boost::gregorian::date& valid_until);
    explicit UpdateRegistrarCertification(
            const RegistrarCertificationIdVariant& certification_id,
            int classification,
            const FileIdVariant& eval_file_id);

    UpdateRegistrarCertification& set_valid_until(const boost::gregorian::date& valid_until);
    UpdateRegistrarCertification& set_classification(int classification);
    UpdateRegistrarCertification& set_eval_file_id(const FileIdVariant& eval_file_id);

    unsigned long long exec(OperationContext& ctx) const;
private:
    RegistrarCertificationIdVariant certification_id_;
    boost::optional<boost::gregorian::date> valid_until_;
    boost::optional<int> classification_;
    boost::optional<FileIdVariant> eval_file_id_;
};

RegistrarCertification update_registrar_certification(
        OperationContext& ctx,
        const UpdateRegistrarCertification::RegistrarCertificationIdVariant& certification_id,
        const boost::optional<boost::gregorian::date>& valid_until,
        const boost::optional<int>& classification,
        const boost::optional<UpdateRegistrarCertification::FileIdVariant>& eval_file_id);

} // namespace Registrar
} // namespace LibFred

#endif
