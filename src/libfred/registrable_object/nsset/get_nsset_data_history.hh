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

#ifndef GET_NSSET_DATA_HISTORY_HH_AFA442B2A1ED43DB8ACD9E120E86C86B
#define GET_NSSET_DATA_HISTORY_HH_AFA442B2A1ED43DB8ACD9E120E86C86B

#include "libfred/registrable_object/nsset/nsset_data_history.hh"
#include "libfred/registrable_object/nsset/nsset_uuid.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Nsset {

class GetNssetDataHistoryById
{
public:
    explicit GetNssetDataHistoryById(unsigned long long nsset_id);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    unsigned long long nsset_id_;
};

class GetNssetDataHistoryByHandle
{
public:
    explicit GetNssetDataHistoryByHandle(const std::string& nsset_handle);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    std::string handle_;
};

class GetNssetDataHistoryByUuid
{
public:
    explicit GetNssetDataHistoryByUuid(const NssetUuid& nsset_uuid);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    NssetUuid uuid_;
};

} // namespace LibFred::RegistrableObject::Nsset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
