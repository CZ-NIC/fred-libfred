/*
 * Copyright (C) 2018-2021  CZ.NIC, z. s. p. o.
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
#ifndef GET_CONTACT_STATE_HISTORY_HH_7827DF8F59926B83B7C006F9DD976553//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define GET_CONTACT_STATE_HISTORY_HH_7827DF8F59926B83B7C006F9DD976553

#include "libfred/registrable_object/contact/contact_state_history.hh"
#include "libfred/registrable_object/contact/contact_uuid.hh"
#include "libfred/registrable_object/history_interval.hh"
#include "libfred/registrable_object/exceptions.hh"
#include "libfred/opcontext.hh"

namespace LibFred {
namespace RegistrableObject {
namespace Contact {

class GetContactStateHistoryById
{
public:
    explicit GetContactStateHistoryById(unsigned long long contact_id);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactStateHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(const OperationContext& ctx, const HistoryInterval& range)const;
private:
    unsigned long long contact_id_;
};

class GetContactStateHistoryByHandle
{
public:
    explicit GetContactStateHistoryByHandle(const std::string& contact_handle);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactStateHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(const OperationContext& ctx, const HistoryInterval& range)const;
private:
    std::string handle_;
};

class GetContactStateHistoryByUuid
{
public:
    explicit GetContactStateHistoryByUuid(const ContactUuid& contact_uuid);
    static constexpr auto object_type = Object_Type::contact;
    using Result = ContactStateHistory;
    using DoesNotExist = ObjectDoesNotExist<object_type>;
    using InvalidHistoryIntervalSpecification = RegistrableObject::InvalidHistoryIntervalSpecification<object_type>;
    Result exec(const OperationContext& ctx, const HistoryInterval& range)const;
private:
    ContactUuid uuid_;
};

}//namespace LibFred::RegistrableObject::Contact
}//namespace LibFred::RegistrableObject
}//namespace LibFred

#endif//GET_CONTACT_STATE_HISTORY_HH_7827DF8F59926B83B7C006F9DD976553
