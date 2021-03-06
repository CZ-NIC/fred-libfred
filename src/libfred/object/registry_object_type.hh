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
 *  basic object types in registry
 */

#ifndef REGISTRY_OBJECT_TYPE_HH_0EECBD38151F4C7BAF98139B9BFAD276
#define REGISTRY_OBJECT_TYPE_HH_0EECBD38151F4C7BAF98139B9BFAD276

#include <string>

namespace LibFred {

/* TODO ready for discussion about more appropriate "enum" implementation */

enum object_type {
    contact,
    domain,
    keyset,
    nsset
};

struct ExceptionUnknownObjectType {
    const char* what() const { return "unknown object type"; }
};

/** @throws ExceptionUnknownObjectType */
inline std::string to_db_handle(object_type _input) {
    switch(_input) {
        case contact:   return "contact";
        case domain:    return "domain";
        case keyset:    return "keyset";
        case nsset:     return "nsset";
    };

    throw ExceptionUnknownObjectType();
}

/** @throws ExceptionUnknownObjectType */
inline object_type object_type_from_db_handle(const std::string& _handle) {
    if (_handle == "contact" ) { return contact; }
    if (_handle == "domain"  ) { return domain;  }
    if (_handle == "keyset"  ) { return keyset;  }
    if (_handle == "nsset"   ) { return nsset;   }

    throw ExceptionUnknownObjectType();
}

}

#endif
