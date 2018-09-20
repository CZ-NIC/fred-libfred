/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STATE_HH_D50CE7E35AB8802662C58685DEC83AD8//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STATE_HH_D50CE7E35AB8802662C58685DEC83AD8

#include "libfred/object/object_type.hh"

#include "util/flagset.hh"

namespace LibFred {
namespace RegistrableObject {

struct State
{
    struct Flag
    {
        enum class Manipulation
        {
            manual,
            automatic
        };
        enum class Visibility
        {
            external,
            internal
        };
        template <Manipulation m, Visibility v, const char* n>
        struct Type
        {
            static constexpr const char* name = n;
            static constexpr Manipulation how_to_set = m;
            static constexpr Visibility visibility = v;
        };
        template <const char* n>
        using AutomaticExternal = Type<Manipulation::automatic, Visibility::external, n>;
        template <const char* n>
        using AutomaticInternal = Type<Manipulation::automatic, Visibility::internal, n>;
        template <const char* n>
        using ManualExternal = Type<Manipulation::manual, Visibility::external, n>;
        template <const char* n>
        using ManualInternal = Type<Manipulation::manual, Visibility::internal, n>;
    };

    template <Object_Type::Enum o, typename ...Flags>
    struct Of
    {
        static constexpr Object_Type::Enum object_type = o;
        using Type = Util::FlagSet<Of, Flags...>;
    };
};

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//STATE_HH_D50CE7E35AB8802662C58685DEC83AD8
