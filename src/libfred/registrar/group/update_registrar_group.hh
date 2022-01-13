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
 *  update registrar group
 */

#ifndef UPDATE_REGISTRAR_GROUP_HH_AC7B3AE57698429597EAE7BD1F3E66D2
#define UPDATE_REGISTRAR_GROUP_HH_AC7B3AE57698429597EAE7BD1F3E66D2

#include "libfred/opcontext.hh"

#include <string>

namespace LibFred {
namespace Registrar {

class UpdateRegistrarGroup
{
public:
    UpdateRegistrarGroup(unsigned long long _group_id,
                         const std::string& _group_name)
    : group_id_(_group_id),
      group_name_(_group_name)
    {}

    void exec(const OperationContext& _ctx);

private:
    const unsigned long long group_id_;
    const std::string group_name_;

};

} // namespace Registrar
} // namespace LibFred

#endif
