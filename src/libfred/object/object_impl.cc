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
 *  @file
 *  object impl
 */


#include <string>

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"

#include "libfred/object/object_impl.hh"

namespace LibFred
{

    unsigned long long get_object_type_id(const OperationContext& ctx, const std::string& obj_type)
    {
        const Database::Result object_type_res = ctx.get_conn().exec_params(
            "SELECT id FROM enum_object_type WHERE name = $1::text FOR SHARE"
            , Database::query_param_list(obj_type));
        if (object_type_res.size() != 1)
        {
            BOOST_THROW_EXCEPTION(InternalError("failed to get object type"));
        }
        return  static_cast<unsigned long long> (object_type_res[0][0]);
    }

} // namespace LibFred

