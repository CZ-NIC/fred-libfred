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
#ifndef STATE_FLAG_SETTER_HH_EA79593F14AAC1DCF44D7435DF011C8A//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STATE_FLAG_SETTER_HH_EA79593F14AAC1DCF44D7435DF011C8A

#include "util/flagset.hh"

namespace LibFred {
namespace RegistrableObject {

template <typename ...Flags>
class StateFlagSetter
{
public:
    explicit StateFlagSetter(const std::string& flag_name) : flag_name_(flag_name) { }
    template <typename F, int, typename T>
    Util::FlagSetVisiting visit(T& status)
    {
        if (F::name == flag_name_)
        {
            status.template set<F>();
            return Util::FlagSetVisiting::is_done;
        }
        return Util::FlagSetVisiting::can_continue;
    }
private:
    const std::string flag_name_;
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//STATE_FLAG_SETTER_HH_EA79593F14AAC1DCF44D7435DF011C8A
