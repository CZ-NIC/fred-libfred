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
 *  get registrar certifications
 */

#ifndef GET_REGISTRAR_CERTIFICATIONS_HH_FD81A9953AF44D95B8C7F3F87BF3F249
#define GET_REGISTRAR_CERTIFICATIONS_HH_FD81A9953AF44D95B8C7F3F87BF3F249

#include "libfred/opcontext.hh"
#include "libfred/registrar/certification/registrar_certification_type.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {
namespace Registrar {

class GetRegistrarCertifications
{
public:
    explicit GetRegistrarCertifications(unsigned long long _registrar_id);

    std::vector<RegistrarCertification> exec(const OperationContext& _ctx) const;

private:
    unsigned long long registrar_id_;

};

} // namespace Registrar
} // namespace LibFred

#endif
