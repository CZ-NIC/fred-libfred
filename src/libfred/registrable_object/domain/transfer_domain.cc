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
#include "libfred/registrable_object/domain/transfer_domain.hh"

#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrable_object/domain/copy_history_impl.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/object/generate_authinfo_password.hh"
#include "libfred/object/transfer_object.hh"

#include "libfred/exception.hh"

namespace LibFred
{

    TransferDomain::TransferDomain(
        const unsigned long long _domain_id,
        const std::string& _new_registrar_handle,
        const std::string& _authinfopw_for_authorization,
        const Nullable<unsigned long long>& _logd_request_id
    ) :
        domain_id_(_domain_id),
        new_registrar_handle_(_new_registrar_handle),
        authinfopw_for_authorization_(_authinfopw_for_authorization),
        logd_request_id_(_logd_request_id)
    { }

    /**
     * @returns true if _authinfopw_for_authorization is correct
     * @throws UnknownDomainId
     */
    static bool is_transfer_authorized(OperationContext& _ctx, const unsigned long long _domain_id, const std::string& _authinfopw_for_authorization) {

        LibFred::InfoDomainData domain_data;

        try {
            domain_data = LibFred::InfoDomainById(_domain_id).set_lock().exec(_ctx).info_domain_data;

        } catch(const LibFred::InfoDomainById::Exception& e) {
            if (e.is_set_unknown_object_id() ) {
                throw UnknownDomainId();
            }
            throw;
        }

        if (domain_data.authinfopw == _authinfopw_for_authorization) {
            return true;
        }

        if (InfoContactByHandle(domain_data.registrant.handle).exec(_ctx).info_contact_data.authinfopw
            == _authinfopw_for_authorization
        ) {
            return true;
        }

        for (const auto& admin_contact : domain_data.admin_contacts) {
            if (InfoContactByHandle(admin_contact.handle).exec(_ctx).info_contact_data.authinfopw
                == _authinfopw_for_authorization
            ) {
                return true;
            }
        }

        return false;
    }

    unsigned long long TransferDomain::exec(OperationContext& _ctx) {

        if (is_transfer_authorized(_ctx, domain_id_, authinfopw_for_authorization_) ) {
            unsigned long long new_history_id;

            try {
                new_history_id = LibFred::transfer_object(_ctx, domain_id_, new_registrar_handle_, generate_authinfo_pw(), logd_request_id_ );

            } catch(const UnknownObjectId& e) {
                throw UnknownDomainId();
            }

            copy_domain_data_to_domain_history_impl(_ctx, domain_id_, new_history_id);

            return new_history_id;

        } else {
            throw IncorrectAuthInfoPw();
        }
    }
}
