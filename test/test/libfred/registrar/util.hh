/*
 * Copyright (C) 2020-2022  CZ.NIC, z. s. p. o.
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
#ifndef UTIL_HH_2EE13D72AC394742A492372492CD6F20
#define UTIL_HH_2EE13D72AC394742A492372492CD6F20

#include "libfred/opcontext.hh"
#include "test/setup/fixtures.hh"

namespace Test {

struct ContextHolder : virtual instantiate_db_template
{
    LibFred::OperationContextCreator ctx;
};

template <class T>
struct SupplyFixtureCtx : ContextHolder, T
{
    SupplyFixtureCtx()
        : ContextHolder(),
          T(ctx)
    {
    }
};

} // namespace Test

#endif
