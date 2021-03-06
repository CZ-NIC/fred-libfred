/*
 * Copyright (C) 2020  CZ.NIC, z. s. p. o.
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
#include "libfred/db_settings.hh"
#include "libfred/registrar/epp_auth/add_registrar_epp_auth.hh"
#include "util/password_storage.hh"
#include "util/password_storage/password_data.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/libfred/registrar/epp_auth/util.hh"

#include <stdexcept>

namespace Test {

unsigned long long get_epp_auth_id(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const boost::optional<std::string>& _plain_password)
{
    const Database::Result db_result = _ctx.get_conn().exec_params(
            "SELECT ra.id,ra.password "
            "FROM registraracl ra "
            "JOIN registrar r ON r.id=ra.registrarid "
            "WHERE r.handle=$1::text AND "
                  "ra.cert=$2::text",
            Database::query_param_list(_registrar_handle)
                                      (_certificate_fingerprint));

    if (db_result.size() == 1)
    {
        const auto id = static_cast<unsigned long long>(db_result[0][0]);
        if (_plain_password != boost::none)
        {
            const auto hashed_password =
                    ::PasswordStorage::PasswordData::construct_from(static_cast<std::string>(db_result[0][1]));
            ::PasswordStorage::check_password(*_plain_password, hashed_password);
        }
        return id;
    }
    if (db_result.size() == 0)
    {
        return 0;
    }
    throw std::runtime_error("Duplicate EPP authentications have been found in the database.");
}

unsigned long long add_epp_authentications(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password)
{
    const unsigned long long id = ::LibFred::Registrar::EppAuth::AddRegistrarEppAuth(
            _registrar_handle,
            _certificate_fingerprint,
            _plain_password).exec(_ctx);

    const unsigned size = Random::Generator().get(1, 6);
    for (unsigned i = 0; i < size; ++i)
    {
        const std::string cert = Random::Generator().get_seq(Random::CharSet::letters(), 20);
        const std::string pass = Random::Generator().get_seq(Random::CharSet::letters(), 10);
        ::LibFred::Registrar::EppAuth::AddRegistrarEppAuth(_registrar_handle, cert, pass).exec(_ctx);
    }
    return id;
}

}//namespace Test
