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

#ifndef CHECK_AUTHINFO_HH_270358FE1781502A7996BB61D1D69342//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CHECK_AUTHINFO_HH_270358FE1781502A7996BB61D1D69342

#include "libfred/exception.hh"
#include "libfred/opcontext.hh"
#include "libfred/object/types.hh"

#include "util/printable.hh"

#include <string>

namespace LibFred {
namespace Object {

/**
 * Check if password match any authinfo associated with given object.
 */
class CheckAuthinfo : public Util::Printable<CheckAuthinfo>
{
public:
    /**
     * CheckAuthinfo constructor with mandatory parameters.
     * @param object_id object's reference
     */
    explicit CheckAuthinfo(ObjectId object_id);

    using Visitor = void(*)(const OperationContext&, const AuthinfoId&);

    /**
     * Check if password match any authinfo associated with given object.
     * @param ctx database connection
     * @param plaintext_password password in plain text format
     * @param on_match procedure called if password match an authinfo
     * @return number of authinfos associated with given object and matching the password
     * @throws UnknownObject in case of object_id does not refer any existing registrable object
     * @throws InternalError otherwise
     */
    int exec(const OperationContext& ctx, const std::string& plaintext_password, Visitor on_match) const;

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string() const;

    static void increment_usage(const OperationContext& ctx, const AuthinfoId& authinfo_id);
    static void increment_usage_and_cancel(const OperationContext& ctx, const AuthinfoId& authinfo_id);
private:
    ObjectId object_id_;
};

}//namespace LibFred::Object
}//namespace LibFred

#endif//CHECK_AUTHINFO_HH_270358FE1781502A7996BB61D1D69342
