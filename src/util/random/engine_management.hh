/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#ifndef ENGINE_INSTANCE_HH_70714B11B7BA41B5AB246FCC95E9B2B9
#define ENGINE_INSTANCE_HH_70714B11B7BA41B5AB246FCC95E9B2B9

#include "details/create_engine.hh"

namespace Random {

// TODO: discuss whether to make this thread-safe, remove it,
// or keep it as it is
struct GlobalEngineInstance
{
    template<typename E, typename S>
    static E& get_engine()
    {
        static E instance = Details::create_engine<E, S>();
        return instance;
    }
};

struct ThreadLocalEngineInstance
{
    template<typename E, typename S>
    static E& get_engine()
    {
        thread_local E instance = Details::create_engine<E, S>();
        return instance;
    }
};

} // namespace Random

#endif
