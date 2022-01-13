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
/**
 *  @file check_handle.hh
 *  handle check
 */

#ifndef CHECK_HANDLE_HH_86E459F246F043259DB898F445EEB82A
#define CHECK_HANDLE_HH_86E459F246F043259DB898F445EEB82A

#include "libfred/opcontext.hh"
#include "libfred/object/object_type.hh"

#include <boost/mpl/assert.hpp>

namespace LibFred {

template <Object_Type::Enum TYPE_OF_OBJECT>
class TestHandleOf
{
public:
    TestHandleOf(const std::string& _handle);

    //check handle syntax
    bool is_invalid_handle(const OperationContext& _ctx)const;

    //check if handle is in protected period
    bool is_protected(const OperationContext& _ctx)const;

    //check if handle is already registered
    bool is_registered(const OperationContext& _ctx)const;
private:
    const std::string handle_;
    BOOST_MPL_ASSERT_MSG((TYPE_OF_OBJECT != Object_Type::domain) && //domain handle is called "fqdn" => no handle operations for domains
                         ((TYPE_OF_OBJECT == Object_Type::contact) ||
                          (TYPE_OF_OBJECT == Object_Type::nsset)   ||
                          (TYPE_OF_OBJECT == Object_Type::keyset)),
                         unavailable_for_this_type_of_object, (Object_Type::Enum));
};

}//namespace LibFred

#endif//CHECK_HANDLE_HH_86E459F246F043259DB898F445EEB82A
