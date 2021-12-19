/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
/**
 *  @file
 *  create registrar certification
 */

#ifndef CREATE_REGISTRAR_CERTIFICATION_HH_BDE927161AD44CEFAEDD7C35AE6E8C28
#define CREATE_REGISTRAR_CERTIFICATION_HH_BDE927161AD44CEFAEDD7C35AE6E8C28

#include "libfred/opcontext.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {
namespace Registrar {

class CreateRegistrarCertification
{
public:
    CreateRegistrarCertification(
            unsigned long long _registrar_id,
            boost::gregorian::date _valid_from,
            int _classification,
            unsigned long long _eval_file_id);

    CreateRegistrarCertification& set_valid_until(boost::gregorian::date _valid_until);

    unsigned long long exec(const OperationContext& _ctx) const;

private:
    unsigned long long registrar_id_;
    boost::gregorian::date valid_from_;
    boost::gregorian::date valid_until_;
    int classification_;
    unsigned long long eval_file_id_;
};

} // namespace Registrar
} // namespace LibFred

#endif
