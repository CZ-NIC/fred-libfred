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
#include "util/log/context.hh"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

#include <vector>
#include <map>

#include <ostream>
#include <sstream>
#include <iterator>

namespace Logging {

namespace {

const std::string& get_ndc_separator()
{
    static const std::string separator = "/";
    return separator;
}

struct NDCData
{
    std::string name;
    std::string path;
};

typedef std::vector<NDCData> Stack;
typedef std::map<std::string, std::string> Map;

struct PerThreadData
{
    Stack stack;
    Map map;
};

/**
 * storage init
 */
boost::thread_specific_ptr<PerThreadData> data;

PerThreadData* get_thread_data()
{
    PerThreadData* const result = data.get();
    if (result != nullptr)
    {
        return result;
    }
    data.reset(new PerThreadData());
    return data.get();
}

/**
 * getters for thread local context data
 */
Stack* get_thread_stack()
{
    return &(get_thread_data()->stack);
}

Map* get_thread_map()
{
    return &(get_thread_data()->map);
}

}//namespace Logging::{anonymous}

Context::Context() { }

Context::Context(const std::string& _name)
{
    push(_name);
}

Context::~Context()
{
    pop();
}

void Context::push(const std::string& _name)
{
    Stack* const stack = get_thread_stack();

    NDCData new_ndc;
    new_ndc.name = _name;
    new_ndc.path = (!stack->empty() ? stack->back().path + get_ndc_separator()
                                    : "") + _name;
    stack->push_back(new_ndc);
}

void Context::pop()
{
    Stack* const stack = get_thread_stack();
    if (!stack->empty())
    {
        stack->pop_back();
    }
}

std::string Context::top()
{
    Stack* const stack = get_thread_stack();

    if (!stack->empty())
    {
        return stack->back().name;
    }
    return std::string();
}

std::string Context::getNDC()
{
    Stack* const stack = get_thread_stack();
    return !stack->empty() ? stack->back().path : std::string();
}

void Context::add(const std::string& _attr, const std::string& _val)
{
    Map* const map = get_thread_map();

    map->emplace(_attr, _val);
}

void Context::rem(const std::string& _attr)
{
    Map* const map = get_thread_map();

    Map::iterator it = map->find(_attr);
    if (it != map->end())
    {
        map->erase(it);
    }
}

std::string Context::getMDC()
{
    Map* const map = get_thread_map();

    std::string full_ctx;
    for (const auto& attr_val : *map)
    {
        full_ctx += (full_ctx.empty() ? "" : " ") + attr_val.first + "=" + attr_val.second;
    }
    return full_ctx;
}

std::string Context::get()
{
    return getNDC() + getMDC();
}

void Context::clear()
{
    PerThreadData* const thread_data = get_thread_data();
    thread_data->stack.clear();
    thread_data->map.clear();
}

}//namespace Logging
