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
#ifndef HISTORY_INTERVAL_IMPL_HH_BEDBD95B9AABD5A42B3B634237F73A97//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define HISTORY_INTERVAL_IMPL_HH_BEDBD95B9AABD5A42B3B634237F73A97

#include "libfred/registrable_object/history_interval.hh"

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
InvalidHistoryIntervalSpecification<o>::InvalidHistoryIntervalSpecification()
    : std::runtime_error("Invalid specification of history interval")
{ }

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//HISTORY_INTERVAL_IMPL_HH_BEDBD95B9AABD5A42B3B634237F73A97
