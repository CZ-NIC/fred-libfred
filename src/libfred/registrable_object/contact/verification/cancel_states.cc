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
#include "libfred/registrable_object/contact/verification/cancel_states.hh"
#include "libfred/object_state/cancel_object_state_request_id.hh"
#include "libfred/object/object_state.hh"

#include <set>

namespace LibFred {
namespace Contact {
namespace Verification {

bool conditionally_cancel_final_states(
        const LibFred::OperationContext& ctx,
        unsigned long long contact_id,
        bool name_changed,
        bool organization_changed,
        bool street1_changed,
        bool street2_changed,
        bool street3_changed,
        bool city_changed,
        bool stateorprovince_changed,
        bool postalcode_changed,
        bool country_changed,
        bool telephone_changed,
        bool fax_changed,
        bool email_changed,
        bool notifyemail_changed,
        bool vat_changed,
        bool ssntype_changed,
        bool ssn_changed)
{
    if (name_changed ||
        organization_changed ||
        street1_changed ||
        street2_changed ||
        street3_changed ||
        city_changed ||
        stateorprovince_changed ||
        postalcode_changed ||
        country_changed ||
        telephone_changed ||
        fax_changed ||
        email_changed ||
        notifyemail_changed ||
        vat_changed ||
        ssntype_changed ||
        ssn_changed)
    {
        cancel_final_states(ctx, contact_id);
        return true;
    }
    return false;
}

namespace {

std::set<Object_State::Enum> get_all()
{
    return { Object_State::contact_in_manual_verification,
             Object_State::contact_passed_manual_verification,
             Object_State::contact_failed_manual_verification };
}

std::set<Object_State::Enum> get_final()
{
    return { Object_State::contact_passed_manual_verification,
             Object_State::contact_failed_manual_verification };
}

void cancel_states(
        const LibFred::OperationContext& _ctx,
        unsigned long long _contact_id,
        const std::set<Object_State::Enum>& _states)
{
    _ctx.get_conn().exec("SAVEPOINT state_savepoint");

    // cancel one state at a time because when exception is thrown, all changes would be ROLLBACKed
    for (const auto object_state : _states)
    {
        try
        {
            LibFred::CancelObjectStateRequestId(
                    _contact_id,
                    { Conversion::Enums::to_db_handle(object_state) }).exec(_ctx);
            _ctx.get_conn().exec("RELEASE SAVEPOINT state_savepoint");
            _ctx.get_conn().exec("SAVEPOINT state_savepoint");
        }
        catch (const LibFred::CancelObjectStateRequestId::Exception& e)
        {
            // in case it throws with unknown cause
            if (!e.is_set_state_not_found())
            {
                throw;
            }
            _ctx.get_conn().exec("ROLLBACK TO state_savepoint");
        }
    }
}

}//namspace LibFred::Contact::Verification::{anonymous}

void cancel_all_states(const LibFred::OperationContext& _ctx, unsigned long long _contact_id)
{
    cancel_states(_ctx, _contact_id, get_all());
}

void cancel_final_states(const LibFred::OperationContext& _ctx, unsigned long long _contact_id)
{
    cancel_states(_ctx, _contact_id, get_final());
}

bool conditionally_cancel_final_states(
        const LibFred::OperationContext& ctx,
        unsigned long long contact_id)
{
    // is there any change?
    const Database::Result result = ctx.get_conn().exec_params(
            "SELECT (c.name IS DISTINCT FROM c_h_before.name) OR "
                   "(c.organization IS DISTINCT FROM c_h_before.organization) OR "
                   "(c.street1 IS DISTINCT FROM c_h_before.street1) OR "
                   "(c.street2 IS DISTINCT FROM c_h_before.street2) OR "
                   "(c.street3 IS DISTINCT FROM c_h_before.street3) OR "
                   "(c.city IS DISTINCT FROM c_h_before.city) OR "
                   "(c.stateorprovince IS DISTINCT FROM c_h_before.stateorprovince) OR "
                   "(c.postalcode IS DISTINCT FROM c_h_before.postalcode) OR "
                   "(c.country IS DISTINCT FROM c_h_before.country) OR "
                   "(c.telephone IS DISTINCT FROM c_h_before.telephone) OR "
                   "(c.fax IS DISTINCT FROM c_h_before.fax) OR "
                   "(c.email IS DISTINCT FROM c_h_before.email) OR "
                   "(c.notifyemail IS DISTINCT FROM c_h_before.notifyemail) OR "
                   "(c.vat IS DISTINCT FROM c_h_before.vat) OR "
                   "(c.ssn IS DISTINCT FROM c_h_before.ssn) OR "
                   "(c.ssntype IS DISTINCT FROM c_h_before.ssntype) "
            "FROM object_registry AS o_r "
            "JOIN contact AS c USING(id) "
            "JOIN history AS h_before ON h_before.next=o_r.historyid "
            "JOIN contact_history AS c_h_before ON c_h_before.historyid=h_before.id "
            "WHERE o_r.id=$1::bigint",
            Database::query_param_list(contact_id));

    if (!static_cast<bool>(result[0][0]))
    {
        return false;
    }
    const std::set<Object_State::Enum> final_states = get_final();
    std::set<std::string> final_states_names;
    for (const auto state : final_states)
    {
        final_states_names.insert(Conversion::Enums::to_db_handle(state));
    }
    try
    {
        LibFred::CancelObjectStateRequestId(contact_id, final_states_names).exec(ctx);
        return true;
    }
    catch (const LibFred::CancelObjectStateRequestId::Exception& e)
    {
        if (e.is_set_state_not_found() && !e.is_set_object_id_not_found())
        {
            /* swallow it - means that the state just wasn't set and nothing else */
            return true;
        }
        throw;
    }
}

}//namespace LibFred::Contact::Verification
}//namespace LibFred::Contact
}//namespace LibFred
