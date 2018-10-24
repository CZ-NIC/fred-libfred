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

#ifndef GET_STATE_IMPL_HH_5459476FE727963D8ADC957831E32B9E//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_STATE_IMPL_HH_5459476FE727963D8ADC957831E32B9E

#include "libfred/registrable_object/get_state.hh"
#include "libfred/registrable_object/exceptions_impl.hh"
#include "libfred/registrable_object/state_flag_setter.hh"

namespace LibFred {
namespace RegistrableObject {

template <typename D, typename S>
typename GetState<D, S>::Result GetState<D, S>::exec(OperationContext& ctx)const
{
    Database::query_param_list params(Conversion::Enums::to_db_handle(object_type));
    const std::string sql =
            "WITH o AS ("
                "SELECT id,type "
                "FROM object_registry "
                "WHERE type=get_object_type_id($1::TEXT) AND "
                      "id=(" + this->derived().get_object_id_rule(params) + ")) "
            "SELECT eos.name "
            "FROM o "
            "LEFT JOIN object_state os ON os.object_id=o.id AND "
                                         "os.valid_to IS NULL "
            "LEFT JOIN enum_object_states eos ON eos.id=os.state_id AND "
                                                "o.type=ANY(eos.types)";
    const auto dbres = ctx.get_conn().exec_params(sql, params);
    if (dbres.size() == 0)
    {
        ctx.get_log().debug(Conversion::Enums::to_db_handle(object_type) + " does not exist");
        throw DoesNotExist();
    }
    Result status;
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][0].isnull();
    if (object_has_state_flags)
    {
        for (std::size_t idx = 0; idx < dbres.size(); ++idx)
        {
            const std::string flag_name = static_cast<std::string>(dbres[idx][0]);
            status.template visit<StateFlagSetter>(flag_name);
        }
    }
    return status;
}

template <typename D, typename S>
const D& GetState<D, S>::derived()const
{
    return *static_cast<const D*>(this);
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATE_IMPL_HH_5459476FE727963D8ADC957831E32B9E
