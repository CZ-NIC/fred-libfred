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
#ifndef GET_NSSET_STATE_HH_2F51E52905A84B0CAED322022F4D81C4
#define GET_NSSET_STATE_HH_2F51E52905A84B0CAED322022F4D81C4

#include "libfred/registrable_object/nsset/nsset_state.hh"
#include "libfred/registrable_object/nsset/nsset_uuid.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Nsset {

class GetNssetStateById
{
public:
    explicit GetNssetStateById(unsigned long long nsset_id);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx) const;
private:
    unsigned long long nsset_id_;
};

class GetNssetStateByHandle
{
public:
    explicit GetNssetStateByHandle(const std::string& fqdn);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx) const;
private:
    std::string fqdn_;
};

class GetNssetStateByUuid
{
public:
    explicit GetNssetStateByUuid(const NssetUuid& nsset_uuid);
    static constexpr auto object_type = Object_Type::nsset;
    using Result = NssetState;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    Result exec(const OperationContext& ctx) const;
private:
    NssetUuid uuid_;
};

} // namespace LibFred::RegistrableObject::Nsset
} // namespace LibFred::RegistrableObject
} // namespace LibFred

#endif
