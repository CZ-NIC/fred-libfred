/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#ifndef CREATE_TRANSACTION_HH_62F4265208F0401885D460DFA6A946FE
#define CREATE_TRANSACTION_HH_62F4265208F0401885D460DFA6A946FE

#include "libfred/opcontext.hh"
#include "util/decimal/decimal.hh"

#include <string>

namespace LibFred {
namespace Registrar {
namespace Credit {

class CreateRegistrarCreditTransaction
{
public:
    CreateRegistrarCreditTransaction(
            const std::string& _registrar,
            const std::string& _zone,
            Decimal _credit_change);

    unsigned long long exec(const OperationContext& _ctx) const;

private:
    std::string registrar_;
    std::string zone_;
    Decimal credit_change_;
};

} // namespace LibFred::Registrar::Credit
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
