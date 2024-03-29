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
/**
 *  @file create_domain_name_blacklist_id.h
 *  create domain name blacklist
 */

#ifndef CREATE_DOMAIN_NAME_BLACKLIST_ID_HH_08D6A20F84844E0389D243C68769039A
#define CREATE_DOMAIN_NAME_BLACKLIST_ID_HH_08D6A20F84844E0389D243C68769039A

#include <boost/date_time/posix_time/posix_time.hpp>

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object_state/typedefs.hh"
#include "util/optional_value.hh"


namespace LibFred
{

    class CreateDomainNameBlacklistId
    {
    public:
        typedef boost::posix_time::ptime Time;
        CreateDomainNameBlacklistId(ObjectId _object_id,
            const std::string &_reason);
        CreateDomainNameBlacklistId(ObjectId _object_id,
            const std::string &_reason,
            const Optional< Time > &_valid_from,
            const Optional< Time > &_valid_to);
        CreateDomainNameBlacklistId& set_valid_from(const Time &_valid_from);
        CreateDomainNameBlacklistId& set_valid_to(const Time &_valid_to);
        void exec(const OperationContext& _ctx);

    //exception impl
        DECLARE_EXCEPTION_DATA(object_id_not_found, ObjectId);
        DECLARE_EXCEPTION_DATA(already_blacklisted_domain, ObjectId);
        DECLARE_EXCEPTION_DATA(out_of_turn, std::string);

        struct Exception
        :   virtual LibFred::OperationException,
            ExceptionData_object_id_not_found<Exception>,
            ExceptionData_already_blacklisted_domain<Exception>,
            ExceptionData_out_of_turn<Exception>
        {};

    private:
        ObjectId object_id_;
        const std::string reason_;
        Optional< Time > valid_from_;
        Optional< Time > valid_to_;
    };//class CreateDomainNameBlacklistId

} // namespace LibFred

#endif
