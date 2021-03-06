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
 *  contact verification exceptions
 */

#ifndef EXCEPTIONS_HH_6C96D662D8B3469BAD7D749F936AA1C7
#define EXCEPTIONS_HH_6C96D662D8B3469BAD7D749F936AA1C7

#include "libfred/opexception.hh"

namespace LibFred
{
    struct ExceptionUnknownContactId : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown contact id";}
    };

    struct ExceptionUnknownTestsuiteHandle : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown testsuite handle";}
    };

    struct ExceptionUnknownCheckHandle : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown check handle";}
    };

    struct ExceptionUnknownTestHandle : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown test handle";}
    };
    struct ExceptionTestNotInMyTestsuite : virtual LibFred::OperationException {
        const char* what() const noexcept {return "test is not in testsuite of this check";}
    };
    struct ExceptionCheckTestPairAlreadyExists : virtual LibFred::OperationException {
        const char* what() const noexcept {return "given check test pair already exists";}
    };

    struct ExceptionUnknownCheckStatusHandle : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown check status handle";}
    };

    struct ExceptionUnknownCheckTestPair : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown check_handle, test_handle pair";}
    };
    struct ExceptionUnknownTestStatusHandle : virtual LibFred::OperationException {
        const char* what() const noexcept {return "unknown test status handle";}
    };
}

#endif
