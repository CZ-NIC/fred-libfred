/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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

#ifndef STORE_AUTHINFO_HH_5C1AC9DBF24B06DF6E6DCA693A70D0F6//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define STORE_AUTHINFO_HH_5C1AC9DBF24B06DF6E6DCA693A70D0F6

#include "libfred/exception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object/types.hh"

#include "util/printable.hh"

#include <chrono>
#include <string>

namespace LibFred {
namespace Object {

struct InvalidTtl : LibFred::Exception { };

/**
 * Store object's authinfo in a secure way.
 */
class StoreAuthinfo : public Util::Printable<StoreAuthinfo>
{
public:
    /**
     * StoreAuthinfo constructor with mandatory parameters.
     * @param object_id object's reference
     * @param registrar_id registrar's reference
     * @param ttl time to password live
     * @throws InvalidTtl in case of TTL is 0 sec
     */
    explicit StoreAuthinfo(
            ObjectId object_id,
            unsigned long long registrar_id,
            std::chrono::seconds ttl);

    /**
     * Store authinfo and return its id.
     * @param ctx database connection
     * @param plaintext_password password in plain text format
     * @return authinfo's id
     * @throws UnknownObject in case of object_id does not refer any existing registrable object
     * @throws UnknownRegistrar in case of registrar_id does not refer any existing registrar
     * @throws InternalError otherwise
     */
    AuthinfoId exec(const OperationContext& ctx, const std::string& plaintext_password) const;

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string() const;
private:
    ObjectId object_id_;
    unsigned long long registrar_id_;
    std::chrono::seconds ttl_;
};

}//namespace LibFred::Object
}//namespace LibFred

#endif//STORE_AUTHINFO_HH_5C1AC9DBF24B06DF6E6DCA693A70D0F6
