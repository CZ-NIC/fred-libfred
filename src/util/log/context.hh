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
#ifndef CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21
#define CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21

#include <string>

namespace Logging {

class Context
{
public:
    Context();

    explicit Context(const std::string& name);

    ~Context();

    Context(const Context&) = delete;

    Context& operator=(const Context&) = delete;

    /**
     * stacked interface implementation
     */
    static void push(const std::string& name);

    static void pop();

    static std::string top();

    static std::string getNDC();

    /**
     * mapped interface implementation
     */
    static void add(const std::string& attr, const std::string& val);

    static void rem(const std::string& attr);

    static std::string getMDC();

    /**
     *  get both contexts - stacked and mapped
     */
    static std::string get();

    static void clear();
};

}//namespace Logging

#endif//CONTEXT_HH_1A3983AA309849AAA28B99D050A00D21
