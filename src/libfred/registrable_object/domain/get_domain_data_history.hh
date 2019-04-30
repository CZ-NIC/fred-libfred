/*
 * Copyright (C) 2019  CZ.NIC, z. s. p. o.
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

#ifndef GET_DOMAIN_DATA_HISTORY_HH_5F86D13CDDB444D99B1D18AD8B6D2679
#define GET_DOMAIN_DATA_HISTORY_HH_5F86D13CDDB444D99B1D18AD8B6D2679

#include "libfred/registrable_object/domain/domain_data_history.hh"
#include "libfred/registrable_object/domain/domain_uuid.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Domain {

class GetDomainDataHistoryById
{
public:
    explicit GetDomainDataHistoryById(unsigned long long domain_id);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    unsigned long long domain_id_;
};

class GetDomainDataHistoryByFqdn
{
public:
    explicit GetDomainDataHistoryByFqdn(const std::string& fqdn);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    std::string handle_;
};

class GetDomainDataHistoryByUuid
{
public:
    explicit GetDomainDataHistoryByUuid(const DomainUuid& domain_uuid);
    static constexpr auto object_type = Object_Type::domain;
    using Result = DomainDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    DomainUuid uuid_;
};

} // namespace LibFred::RegistrableObject::Domain
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
