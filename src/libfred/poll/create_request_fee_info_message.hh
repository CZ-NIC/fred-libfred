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
#ifndef CREATE_REQUEST_FEE_INFO_MESSAGE_HH_B3928B8CAE0F453BA19C4ECBA4EB7CE4
#define CREATE_REQUEST_FEE_INFO_MESSAGE_HH_B3928B8CAE0F453BA19C4ECBA4EB7CE4

#include "libfred/opcontext.hh"
#include "util/decimal/decimal.hh"

#include <boost/date_time/gregorian/gregorian.hpp>

namespace LibFred {
namespace Poll {

class CreateRequestFeeInfoMessage
{
public:
    CreateRequestFeeInfoMessage(
        unsigned long long _registrar_id,
        const boost::posix_time::ptime& _period_from,
        const boost::posix_time::ptime& _period_to,
        unsigned long long _total_free_count,
        unsigned long long _request_count,
        const Decimal& _price,
        const std::string& _time_zone = "Europe/Prague")
    :
        registrar_id_(_registrar_id),
        period_from_(_period_from),
        period_to_(_period_to),
        total_free_count_(_total_free_count),
        request_count_(_request_count),
        price_(_price),
        time_zone_(_time_zone)
    {}

    unsigned long long exec(const OperationContext& _ctx) const;
private:
    unsigned long long registrar_id_;
    boost::posix_time::ptime period_from_;
    boost::posix_time::ptime period_to_;
    unsigned long long total_free_count_;
    unsigned long long request_count_;
    Decimal price_;
    std::string time_zone_;
};

} // namespace LibFred::Poll
} // namespace LibFred

#endif
