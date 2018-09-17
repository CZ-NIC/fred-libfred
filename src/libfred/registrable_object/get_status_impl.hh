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

#ifndef GET_STATUS_IMPL_HH_5459476FE727963D8ADC957831E32B9E//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_STATUS_IMPL_HH_5459476FE727963D8ADC957831E32B9E

#include "libfred/registrable_object/get_status.hh"

#include <string>
#include <type_traits>

namespace LibFred {
namespace RegistrableObject {

template <Object_Type::Enum o>
ObjectNotFound<o>::ObjectNotFound()
    : std::runtime_error(Conversion::Enums::to_db_handle(object_type) + " not found")
{ }

template <typename D, typename S>
template <typename ...Fs>
class GetStatus<D, S>::StatusFlagSetter
{
public:
    explicit StatusFlagSetter(const std::string& flag_name) : flag_name_(flag_name) { }
    template <typename F, int, typename T>
    StatusVisiting visit(T& status)
    {
        static_assert(std::is_same<T, S>::value, "invalid usage");
        if (does_correspond<F>(flag_name_))
        {
            status.template set<F>();
            return StatusVisiting::is_done;
        }
        return StatusVisiting::can_continue;
    }
private:
    const std::string flag_name_;
};

template <typename D, typename S>
typename GetStatus<D, S>::Result GetStatus<D, S>::exec(OperationContext& ctx)const
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
        ctx.get_log().debug(Conversion::Enums::to_db_handle(object_type) + " not found");
        throw NotFound();
    }
    Result status;
    const bool object_has_state_flags = (1 < dbres.size()) || !dbres[0][0].isnull();
    if (object_has_state_flags)
    {
        for (std::size_t idx = 0; idx < dbres.size(); ++idx)
        {
            const std::string flag_name = static_cast<std::string>(dbres[idx][0]);
            status.template visit<StatusFlagSetter>(flag_name);
        }
    }
    return status;
}

template <typename D, typename S>
const D& GetStatus<D, S>::derived()const
{
    return *static_cast<const D*>(this);
}

template <typename S>
GetStatusById<S>::GetStatusById(unsigned long long object_id)
    : object_id_(object_id)
{ }

template <typename S>
std::string GetStatusById<S>::get_object_id_rule(Database::query_param_list& params)const
{
    return "$" + params.add(object_id_) + "::BIGINT";
}

template <typename S>
GetStatusByHandle<S>::GetStatusByHandle(const std::string& handle)
    : handle_(handle)
{ }

template <typename S>
std::string GetStatusByHandle<S>::get_object_id_rule(Database::query_param_list& params)const
{
    static const std::string sql_handle_case_normalize_function =
            S::object_type == Object_Type::domain ? "LOWER"
                                                  : "UPPER";
    return "SELECT id "
           "FROM object_registry "
           "WHERE name=" + sql_handle_case_normalize_function + "($" + params.add(handle_) + "::TEXT) AND "
                 "type=get_object_type_id($" + params.add(Conversion::Enums::to_db_handle(S::object_type)) + "::TEXT) AND "
                 "erdate IS NULL";
}

}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_STATUS_IMPL_HH_5459476FE727963D8ADC957831E32B9E
