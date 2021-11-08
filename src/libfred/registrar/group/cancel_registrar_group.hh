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
 *  cancel registrar group
 */

#ifndef CANCEL_REGISTRAR_GROUP_HH_1EABB2E91CF54AB3BEE934ED74EDD05B
#define CANCEL_REGISTRAR_GROUP_HH_1EABB2E91CF54AB3BEE934ED74EDD05B

#include "libfred/opcontext.hh"

namespace LibFred {
namespace Registrar {

class CancelRegistrarGroup
{
public:
    explicit CancelRegistrarGroup(unsigned long long _group_id)
    : group_id_(_group_id)
    {}

    void exec(const OperationContext& _ctx);

private:
    const unsigned long long group_id_;

};

} // namespace Registrar
} // namespace LibFred

#endif
