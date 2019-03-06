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
#include "util/util.hh"

#include <boost/format.hpp>

namespace Util {

HeadSeparator::HeadSeparator(const std::string& head, const std::string& separator)
    : got_head_(false),
      head_(head),
      separator_(separator)
{ }

std::string HeadSeparator::get()
{
    if (got_head_)
    {
        return separator_;
    }
    got_head_ = true;
    return head_;
}

void HeadSeparator::reset()
{
    got_head_ = false;
}

std::string escape(std::string input, const std::string& what, const std::string& esc_by)
{
    std::size_t idx = 0;
    while ((idx = input.find_first_of(what, idx)) != input.npos)
    {
        input.insert(idx, esc_by);
        idx += esc_by.length() + 1;
    }
    return input;
}

std::string escape2(std::string input)
{
    return escape(input, "'\\", "\\");
}

std::string make_svtrid(unsigned long long request_id)
{
    return boost::str(boost::format("ReqID-%010d") % request_id);
}

}//namespace Util
