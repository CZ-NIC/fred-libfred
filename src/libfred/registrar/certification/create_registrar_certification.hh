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

#ifndef CREATE_REGISTRAR_CERTIFICATION_HH_BDE927161AD44CEFAEDD7C35AE6E8C28//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CREATE_REGISTRAR_CERTIFICATION_HH_BDE927161AD44CEFAEDD7C35AE6E8C28

#include "libfred/registrar/certification/registrar_certification_type.hh"
#include "libfred/opcontext.hh"

#include <boost/variant.hpp>

#include <string>

namespace LibFred {
namespace Registrar {

class CreateRegistrarCertification
{
public:
    using RegistrarIdVariant = boost::variant<unsigned long long, std::string>;
    using FileIdVariant = boost::variant<unsigned long long, FileUuid>;
    explicit CreateRegistrarCertification(
            RegistrarIdVariant _registrar_id,
            boost::gregorian::date _valid_from,
            int _classification,
            FileIdVariant _eval_file_id);
    explicit CreateRegistrarCertification(
            RegistrarIdVariant _registrar_id,
            boost::gregorian::date _valid_from,
            boost::gregorian::date _valid_until,
            int _classification,
            FileIdVariant _eval_file_id);

    CreateRegistrarCertification& set_valid_until(boost::gregorian::date _valid_until);

    unsigned long long exec(OperationContext& _ctx) const;
private:
    RegistrarIdVariant registrar_id_;
    boost::gregorian::date valid_from_;
    boost::gregorian::date valid_until_;
    int classification_;
    FileIdVariant eval_file_id_;
};

RegistrarCertification create_registrar_certification(
        OperationContext& ctx,
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        const boost::gregorian::date& valid_from,
        const boost::gregorian::date& valid_until,
        int classification,
        const CreateRegistrarCertification::FileIdVariant& eval_file_id);

} // namespace Registrar
} // namespace LibFred

#endif//CREATE_REGISTRAR_CERTIFICATION_HH_BDE927161AD44CEFAEDD7C35AE6E8C28
