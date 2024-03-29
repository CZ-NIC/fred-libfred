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

#ifndef UUID_HH_EF6BA55F5AD74813BC76C6832E552E2A
#define UUID_HH_EF6BA55F5AD74813BC76C6832E552E2A

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <cstring>
#include <string>

class uuid
{
public:
    // intentionally not providing default constructor - empty uuid is not valid
    uuid() = delete;
    uuid(uuid&&) = default;
    uuid(const uuid&) = default;
    uuid& operator=(uuid&&) = default;

    /**
     * named constructor
     *
     * @throws ExceptionInvalidUuid
     */
    static uuid from_string(const std::string& _in)
    {
        return uuid{_in};
    }

    explicit uuid(const boost::uuids::uuid& _in)
        : value_{_in}
    { }

    uuid& operator=(const uuid& _rhs)
    {
        value_ = _rhs.value_;
        return *this;
    }

    uuid& operator=(const boost::uuids::uuid& _rhs)
    {
        value_ = _rhs;
        return *this;
    }

    operator std::string() const
    {
        return boost::lexical_cast<std::string>(value_);
    }

    std::string to_string() const
    {
        return static_cast<std::string>(*this);
    }

    struct ExceptionInvalidUuid
    {
        const char* what() const noexcept { return "invalid input UUID"; }
    };
private:
    // the weird syntax is Function Try Block
    explicit uuid(const std::string& _in)
    try
        : value_{boost::uuids::string_generator{}(_in)}
    {
        // cannonical form e. g. 550e8400-e29b-41d4-a716-446655440000
        static constexpr std::string::size_type cannonical_form_length = std::strlen("550e8400-e29b-41d4-a716-446655440000");
        static_assert(cannonical_form_length == 36, "uuid string representation should be 36 characters long");
        if (_in.length() != cannonical_form_length)
        {
            throw ExceptionInvalidUuid{};
        }
    }
    catch (const std::runtime_error&)
    {
        throw ExceptionInvalidUuid{};
    }
    boost::uuids::uuid value_;
};

#endif
