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

#ifndef CLEAN_EXPIRED_AUTHINFOS_HH_C01490AE5B16D00691D2F9F7D613851A//date "+%s.%N"|md5sum|tr "[a-f]" "[A-F]"
#define CLEAN_EXPIRED_AUTHINFOS_HH_C01490AE5B16D00691D2F9F7D613851A

#include "libfred/exception.hh"
#include "libfred/opcontext.hh"

#include "util/printable.hh"

namespace LibFred {
namespace Object {

/**
 * Clean expired authinfos.
 */
class CleanExpiredAuthinfos : public Util::Printable<CleanExpiredAuthinfos>
{
public:
    /**
     * Clean expired authinfos.
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
};

}//namespace LibFred::Object
}//namespace LibFred

#endif//CLEAN_EXPIRED_AUTHINFOS_HH_C01490AE5B16D00691D2F9F7D613851A
