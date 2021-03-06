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
#include "libfred/registrable_object/nsset/transfer_nsset.hh"

#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/copy_history_impl.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/object/generate_authinfo_password.hh"
#include "libfred/object/transfer_object.hh"

#include "libfred/exception.hh"

namespace LibFred
{

    TransferNsset::TransferNsset(
        const unsigned long long _nsset_id,
        const std::string& _new_registrar_handle,
        const std::string& _authinfopw_for_authorization,
        const Nullable<unsigned long long>& _logd_request_id
    ) :
        nsset_id_(_nsset_id),
        new_registrar_handle_(_new_registrar_handle),
        authinfopw_for_authorization_(_authinfopw_for_authorization),
        logd_request_id_(_logd_request_id)
    { }

    /**
     * @returns true if _authinfopw_for_authorization is correct
     * @throws UnknownNssetId
     */
    static bool is_transfer_authorized(OperationContext& _ctx, const unsigned long long _nsset_id, const std::string& _authinfopw_for_authorization) {

        LibFred::InfoNssetData nsset_data;

        try {
            nsset_data = LibFred::InfoNssetById(_nsset_id).set_lock().exec(_ctx).info_nsset_data;

        }
        catch (const LibFred::InfoNssetById::Exception& e) {
            if (e.is_set_unknown_object_id() ) {
                throw UnknownNssetId();
            }
            throw;
        }

        if (nsset_data.authinfopw == _authinfopw_for_authorization) {
            return true;
        }

        for (const auto& tech_contact : nsset_data.tech_contacts) {
            if (InfoContactByHandle(tech_contact.handle).exec(_ctx).info_contact_data.authinfopw
                == _authinfopw_for_authorization
            ) {
                return true;
            }
        }

        return false;
    }

    unsigned long long TransferNsset::exec(OperationContext& _ctx) {

        if (is_transfer_authorized(_ctx, nsset_id_, authinfopw_for_authorization_) ) {
            unsigned long long new_history_id;

            try {
                new_history_id = LibFred::transfer_object(_ctx, nsset_id_, new_registrar_handle_, generate_authinfo_pw(), logd_request_id_ );

            }
            catch (const UnknownObjectId& e) {
                throw UnknownNssetId();
            }

            copy_nsset_data_to_nsset_history_impl(_ctx, nsset_id_, new_history_id);

            return new_history_id;

        } else {
            throw IncorrectAuthInfoPw();
        }
    }
}
