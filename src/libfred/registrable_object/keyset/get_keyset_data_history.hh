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

#ifndef GET_KEYSET_DATA_HISTORY_HH_475CCEB70F5941D2B5055EF16F00E180
#define GET_KEYSET_DATA_HISTORY_HH_475CCEB70F5941D2B5055EF16F00E180

#include "libfred/registrable_object/keyset/keyset_data_history.hh"
#include "libfred/registrable_object/keyset/keyset_uuid.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Keyset {

class GetKeysetDataHistoryById
{
public:
    explicit GetKeysetDataHistoryById(unsigned long long keyset_id);
    static constexpr auto object_type = Object_Type::keyset;
    using Result = KeysetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    unsigned long long keyset_id_;
};

class GetKeysetDataHistoryByHandle
{
public:
    explicit GetKeysetDataHistoryByHandle(const std::string& keyset_handle);
    static constexpr auto object_type = Object_Type::keyset;
    using Result = KeysetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    std::string handle_;
};

class GetKeysetDataHistoryByUuid
{
public:
    explicit GetKeysetDataHistoryByUuid(const KeysetUuid& keyset_uuid);
    static constexpr auto object_type = Object_Type::keyset;
    using Result = KeysetDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range) const;
private:
    KeysetUuid uuid_;
};

} // namespace LibFred::RegistrableObject::Keyset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
