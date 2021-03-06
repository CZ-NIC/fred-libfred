/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
 *  @file
 */

#ifndef ADD_OLD_NEW_SUFFIX_PAIR_HH_1ABFCFFB480A44BC831452CDDBEDF53C
#define ADD_OLD_NEW_SUFFIX_PAIR_HH_1ABFCFFB480A44BC831452CDDBEDF53C

#include "libfred/notifier/exception.hh"

#include <map>
#include <vector>
#include <stdexcept>

namespace Notification {

    /**
     * if _old != _new
     *      insert pair("changes." _key ".old", _old) to _target
     *      insert pair("changes." _key ".new", _new) to _target
     */
inline void add_old_new_changes_pair_if_different(
        std::map<std::string, std::string>& _target,
        const std::string& _key,
        const std::string& _old,
        const std::string& _new)
{
    if (_old != _new)
    {
        if (!_target.insert(std::make_pair("changes." + _key + ".old", _old)).second) /* existing value */
        {
            throw ExceptionInvalidNotificationContent();
        }

        if (!_target.insert(std::make_pair("changes." + _key + ".new", _new)).second) /* existing value */
        {
            throw ExceptionInvalidNotificationContent();
        }

        if (!_target.insert(std::make_pair("changes." + _key, "1")).second) /* existing value */
        {
            throw ExceptionInvalidNotificationContent();
        }
    }
}

}//namespace Notification

#endif//ADD_OLD_NEW_SUFFIX_PAIR_HH_1ABFCFFB480A44BC831452CDDBEDF53C
