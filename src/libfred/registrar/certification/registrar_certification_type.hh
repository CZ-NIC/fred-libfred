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

#ifndef REGISTRAR_CERTIFICATION_TYPE_HH_D0F08A06E53E4022B48B1AA559AE096A
#define REGISTRAR_CERTIFICATION_TYPE_HH_D0F08A06E53E4022B48B1AA559AE096A

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/uuid/uuid.hpp>

#include <tuple>

namespace LibFred {
namespace Registrar {

struct RegistrarCertificationUuid
{
    boost::uuids::uuid value;
};

struct FileUuid
{
    boost::uuids::uuid value;
};

struct RegistrarCertification
{
    unsigned long long id;
    RegistrarCertificationUuid uuid;
    boost::gregorian::date valid_from;
    boost::gregorian::date valid_until;
    int classification;
    unsigned long long eval_file_id;
    FileUuid eval_file_uuid;
    friend bool operator==(const RegistrarCertification& lhs, const RegistrarCertification& rhs)
    {
        static const auto to_tuple = [](const RegistrarCertification& data)
        {
            return std::make_tuple(
                    data.id,
                    data.uuid.value,
                    data.valid_from,
                    data.valid_until,
                    data.classification,
                    data.eval_file_id,
                    data.eval_file_uuid.value);
        };
        return to_tuple(lhs) == to_tuple(rhs);
    }
};

} // namespace Registrar
} // namespace LibFred

#endif
