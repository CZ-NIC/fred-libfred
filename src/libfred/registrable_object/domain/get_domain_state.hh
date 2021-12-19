/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#ifndef GET_DOMAIN_STATE_HH_E0170881B84D411C8E0D6F77143DAF72
#define GET_DOMAIN_STATE_HH_E0170881B84D411C8E0D6F77143DAF72

#include "libfred/registrable_object/domain/domain_state.hh"
#include "libfred/registrable_object/domain/domain_uuid.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

class GetDomainStateById
{
public:
    explicit GetDomainStateById(unsigned long long domain_id);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx)const;
private:
    unsigned long long domain_id_;
};

class GetDomainStateByFqdn
{
public:
    explicit GetDomainStateByFqdn(const std::string& fqdn);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx)const;
private:
    std::string fqdn_;
};

class GetDomainStateByUuid
{
public:
    explicit GetDomainStateByUuid(const DomainUuid& domain_uuid);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx)const;
private:
    DomainUuid uuid_;
};

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
