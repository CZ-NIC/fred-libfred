/*
 * Copyright (C) 2018  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GET_CONTACT_DATA_HISTORY_HH_6D0A72C12AEB01C90DF7BBD93DA5E0EA//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_CONTACT_DATA_HISTORY_HH_6D0A72C12AEB01C90DF7BBD93DA5E0EA

#include "libfred/registrable_object/contact/contact_data_history.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

class GetContactDataHistoryById
{
public:
    explicit GetContactDataHistoryById(unsigned long long contact_id);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    unsigned long long contact_id_;
};

class GetContactDataHistoryByHandle
{
public:
    explicit GetContactDataHistoryByHandle(const std::string& contact_handle);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    std::string handle_;
};

class GetContactDataHistoryByUuid
{
public:
    //TODO: use true uuid when it is available
    explicit GetContactDataHistoryByUuid(unsigned long long contact_uuid);
    explicit GetContactDataHistoryByUuid(const std::string& contact_uuid);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactDataHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(OperationContext& ctx, const HistoryInterval& range)const;
private:
    std::string uuid_;
};

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_CONTACT_DATA_HISTORY_HH_6D0A72C12AEB01C90DF7BBD93DA5E0EA
