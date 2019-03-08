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
#ifndef EXCEPTION_HH_8D9A7C0FA9A941C8B16588CC02BD721A
#define EXCEPTION_HH_8D9A7C0FA9A941C8B16588CC02BD721A

#include <stdexcept>

namespace LibFred
{
    struct Exception : std::exception {
        virtual const char* what() const noexcept = 0;
        virtual ~Exception() { }
    };

    struct IncorrectAuthInfoPw : Exception {
        const char* what() const noexcept { return "incorrect AuthInfoPw"; }
    };

    struct UnknownRegistrar : Exception {
        const char* what() const noexcept { return "unknown registrar"; }
    };

    struct UnknownObjectId : Exception {
        const char* what() const noexcept { return "unknown object id"; }
    };

    struct UnknownContactId : UnknownObjectId {
        const char* what() const noexcept { return "unknown contact id"; }
    };

    struct UnknownDomainId : UnknownObjectId {
        const char* what() const noexcept { return "unknown domain id"; }
    };

    struct UnknownKeysetId : UnknownObjectId {
        const char* what() const noexcept { return "unknown keyset id"; }
    };

    struct UnknownNssetId : UnknownObjectId {
        const char* what() const noexcept { return "unknown nsset id"; }
    };
}

#endif
