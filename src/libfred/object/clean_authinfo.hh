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

#ifndef CLEAN_AUTHINFO_HH_27B8CB6CAAF51918998932BD826FBB21//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CLEAN_AUTHINFO_HH_27B8CB6CAAF51918998932BD826FBB21

#include "libfred/opcontext.hh"
#include "libfred/object/types.hh"

#include "util/printable.hh"

namespace LibFred {
namespace Object {

/**
 * Clean authinfo.
 */
class CleanAuthinfo : public Util::Printable<CleanAuthinfo>
{
public:
    /**
     * CleanAuthinfo constructor with mandatory parameters.
     * @param object_id object's reference
     */
    explicit CleanAuthinfo(ObjectId object_id);

    /**
     * Clean authinfo of given object.
     * @param ctx database connection
     * @return number of cleaned authinfos
     * @throws InternalError in case of error
     */
    int exec(const OperationContext& ctx) const;

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string() const;
private:
    ObjectId object_id_;
};

}//namespace LibFred::Object
}//namespace LibFred

#endif//CLEAN_AUTHINFO_HH_27B8CB6CAAF51918998932BD826FBB21
