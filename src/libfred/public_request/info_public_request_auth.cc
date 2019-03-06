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
#include "libfred/public_request/info_public_request_auth.hh"

namespace LibFred {

PublicRequestAuthInfo::PublicRequestAuthInfo(OperationContext &_ctx, const LockedPublicRequest &_locked)
:   PublicRequestInfo(_ctx, _locked)
{
    try {
        const Database::Result res = _ctx.get_conn().exec_params(
            "SELECT identification,"
                   "password "
            "FROM public_request_auth "
            "WHERE id=$1::BIGINT", Database::query_param_list(id_));
        if (res.size() <= 0) {
            throw std::runtime_error("no public request with authentication found");
        }
        const Database::Row row = res[0];
        identification_ = static_cast< std::string >(row[0]);
        password_       = static_cast< std::string >(row[1]);
    }
    catch (...) {
        throw;
    }
}

PublicRequestAuthInfo::PublicRequestAuthInfo(const PublicRequestAuthInfo &_src)
:   PublicRequestInfo(static_cast< const PublicRequestInfo& >(_src)),
    identification_(_src.identification_),
    password_      (_src.password_)
{ }

PublicRequestAuthInfo& PublicRequestAuthInfo::operator=(const PublicRequestAuthInfo &_src)
{
    static_cast< PublicRequestInfo& >(*this) = static_cast< const PublicRequestInfo& >(_src);
    identification_ = _src.identification_;
    password_       = _src.password_;
    return *this;
}

} // namespace LibFred
