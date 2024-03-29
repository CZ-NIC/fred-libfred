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
 *  @file
 *  create contact test
 */

#ifndef CREATE_TEST_HH_C2D64B58513A456FB1831244EF9B8F44
#define CREATE_TEST_HH_C2D64B58513A456FB1831244EF9B8F44

#include "util/printable.hh"

#include "libfred/registrable_object/contact/verification/exceptions.hh"
#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"
#include "util/optional_value.hh"
#include "util/uuid.hh"

namespace LibFred {

/**
 * Creates new record in contact_test_resutl table with status @ref ContactTestStatus::RUNNING . Has no sideeffects.
 */
class CreateContactTest : public Util::Printable<CreateContactTest>
{
public:
    /**
     * constructor only with mandatory parameters
     * @param _check_handle     identifies which contact_check this test belongs to (by check's handle).
     * @param _test_handle      denotes type of test (by it's handle) to be run. Allowed values are in enum_contact_test.name in database.
     */
    CreateContactTest(
            const uuid& _check_handle,
            const std::string& _test_handle);
    /**
     * constructor with all available parameters including optional ones
     * @param _check_handle     identifies which contact_check this test belongs to (by check's handle).
     * @param _test_handle      denotes type of test to be run. Allowed values are in enum_contact_test.name in database.
     * @param _logd_request_id  identifies optional log entry in logd related to this operation.
     */
    CreateContactTest(
        const uuid& _check_handle,
        const std::string& _test_handle,
        Optional<unsigned long long> _logd_request_id);

    /**
     * setter of optional logd_request_id
     * Call with another value for re-set, no need to unset first.
     */
    CreateContactTest& set_logd_request_id(unsigned long long _logd_request_id);

    /**
     * Commits operation.
     * @throws LibFred::ExceptionUnknownCheckHandle
     * @throws LibFred::ExceptionUnknownTestHandle
     * @throws LibFred::ExceptionTestNotInMyTestsuite
     * @throws LibFred::ExceptionCheckTestPairAlreadyExists
     */
    void exec(const OperationContext& ctx);

    std::string to_string()const;
private:
    uuid check_handle_;
    std::string test_handle_;
    Nullable<unsigned long long> logd_request_id_;
};

}//namespace LibFred

#endif//CREATE_TEST_HH_C2D64B58513A456FB1831244EF9B8F44
