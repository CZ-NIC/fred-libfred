/*
 * Copyright (C) 2019-2022  CZ.NIC, z. s. p. o.
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

#ifndef UPDATE_REGISTRAR_EPP_AUTH_HH_4D17531385D14877B071FDC2D35425A0
#define UPDATE_REGISTRAR_EPP_AUTH_HH_4D17531385D14877B071FDC2D35425A0

#include "libfred/opcontext.hh"
#include "libfred/registrar/epp_auth/registrar_epp_auth_data.hh"

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <string>

namespace LibFred {
namespace Registrar {
namespace EppAuth {

class UpdateRegistrarEppAuth
{
public:
    using EppAuthRecordCommonId = boost::variant<unsigned long long, EppAuthRecordUuid>;
    explicit UpdateRegistrarEppAuth(EppAuthRecordCommonId id);

    UpdateRegistrarEppAuth& set_certificate_fingerprint(
            boost::optional<std::string> certificate_fingerprint);

    UpdateRegistrarEppAuth& set_certificate(
            boost::optional<std::string> certificate_fingerprint,
            boost::optional<std::string> certificate_data_pem);

    UpdateRegistrarEppAuth& set_plain_password(boost::optional<std::string> plain_password);

    void exec(OperationContext& ctx) const;
private:
    EppAuthRecordCommonId id_;
    boost::optional<std::string> certificate_fingerprint_;
    boost::optional<std::string> plain_password_;
    boost::optional<std::string> certificate_data_pem_;
};

EppAuthRecord update_registrar_epp_auth(
        OperationContext& ctx,
        const UpdateRegistrarEppAuth::EppAuthRecordCommonId& id,
        const boost::optional<std::string>& certificate_fingerprint,
        const boost::optional<std::string>& plain_password,
        const boost::optional<std::string>& certificate_data_pem);

} // namespace LibFred::Registrar::EppAuth
} // namespace LibFred::Registrar
} // namespace LibFred

#endif
