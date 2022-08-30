/*
 * Copyright (C) 2020-2022  CZ.NIC, z. s. p. o.
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

#include <boost/optional.hpp>

#include <algorithm>
#include <stdexcept>

using namespace Test;

unsigned long long Test::get_epp_auth_id(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const boost::optional<std::string>& _plain_password)
{
    const Database::Result db_result = _ctx.get_conn().exec_params(
            "SELECT ra.id, "
                   "ra.password "
              "FROM registraracl ra "
              "JOIN registrar r ON r.id = ra.registrarid "
             "WHERE r.handle = UPPER($1::TEXT) AND "
                   "DECODE(REGEXP_REPLACE(ra.cert, ':', '', 'g'), 'hex') = "
                   "DECODE(REGEXP_REPLACE($2::TEXT, ':', '', 'g'), 'hex')",
            Database::QueryParams{_registrar_handle, _certificate_fingerprint});

    if (db_result.size() == 1)
    {
        const auto id = static_cast<unsigned long long>(db_result[0][0]);
        if ((_plain_password != boost::none) && !_plain_password->empty())
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
    struct UnexpectedNumberOfRows : std::runtime_error
    {
        explicit UnexpectedNumberOfRows() : std::runtime_error{{}} { }
        const char* what() const noexcept override { return "Duplicate EPP authentications have been found in the database."; }
    };
    throw UnexpectedNumberOfRows{};
}

unsigned long long Test::get_epp_auth_id(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password)
{
    return get_epp_auth_id(_ctx, _registrar_handle, _certificate_fingerprint, boost::make_optional(_plain_password));
}

unsigned long long Test::add_epp_authentications(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password)
{
    return add_epp_authentications(
            _ctx,
            _registrar_handle,
            _certificate_fingerprint,
            _plain_password,
            std::string{}).id;
}

::LibFred::Registrar::EppAuth::EppAuthRecord Test::add_epp_authentications(
        ::LibFred::OperationContext& _ctx,
        const std::string& _registrar_handle,
        const std::string& _certificate_fingerprint,
        const std::string& _plain_password,
        const std::string& _certificate_data_pem)
{
    auto result = ::LibFred::Registrar::EppAuth::add_registrar_epp_auth(
            _ctx,
            _registrar_handle,
            _certificate_fingerprint,
            _plain_password,
            _certificate_data_pem);

    const unsigned size = Random::Generator().get(1, 6);
    for (unsigned i = 0; i < size; ++i)
    {
        ::LibFred::Registrar::EppAuth::AddRegistrarEppAuth{
                _registrar_handle,
                get_random_fingerprint(),
                Random::Generator().get_seq(Random::CharSet::letters(), 10),
                _certificate_data_pem}.exec(_ctx);
    }
    return result;
}

std::string Test::get_random_fingerprint()
{
    static constexpr auto fingerprint_bytes = 16;
    static_assert(0 < fingerprint_bytes);
    static constexpr auto number_of_delimiters = fingerprint_bytes - 1;
    static constexpr auto delimiter = ':';
    const auto hex_str = Random::Generator().get_seq("0123456789ABCDEF", 2 * fingerprint_bytes);
    std::string result;
    result.reserve(2 * fingerprint_bytes + number_of_delimiters);
    std::size_t idx = 0;
    std::for_each(begin(hex_str), end(hex_str), [&](char c)
    {
        if ((0 < idx) && ((idx % 2) == 0))
        {
            result.append({delimiter, c});
        }
        else
        {
            result.append({c});
        }
        ++idx;
    });
    return result;
}
