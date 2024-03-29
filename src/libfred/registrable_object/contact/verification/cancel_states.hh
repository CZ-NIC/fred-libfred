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
 *  delete all contact states related to admin contact verification
 */

#ifndef CANCEL_STATES_HH_187A1478735E49EC93BEE97776D5CF35
#define CANCEL_STATES_HH_187A1478735E49EC93BEE97776D5CF35

#include "libfred/opcontext.hh"
#include "util/optional_value.hh"

#include <string>

namespace LibFred {
namespace Contact {
namespace Verification {

/* OperationContext versions */

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
        bool ssn_changed);

/** @returns true if contact has changed data related to verification false otherwise */
bool conditionally_cancel_final_states(
        const LibFred::OperationContext& ctx,
        unsigned long long contact_id);

void cancel_all_states(
        const LibFred::OperationContext& ctx,
        unsigned long long contact_id);

void cancel_final_states(
        const LibFred::OperationContext& ctx,
        unsigned long long contact_id);

}//namespace LibFred::Contact::Verification
}//namespace LibFred::Contact
}//namespace LibFred

#endif//CANCEL_STATES_HH_187A1478735E49EC93BEE97776D5CF35
