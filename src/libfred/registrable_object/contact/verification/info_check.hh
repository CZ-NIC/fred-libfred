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
 *  (get) info (about) contact check
 */

#ifndef INFO_CHECK_HH_78A1015DB35948E7AFA58CF421DE454F
#define INFO_CHECK_HH_78A1015DB35948E7AFA58CF421DE454F

#include "util/printable.hh"

#include "libfred/registrable_object/contact/verification/exceptions.hh"
#include "libfred/opcontext.hh"
#include "util/db/nullable.hh"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace LibFred {

/**
 * Return structure of operation @ref InfoContactCheck. Includes type definition of it's own parts.
 * All times are returned as local @ref InfoContactCheck::exec().
 */
struct InfoContactCheckOutput
{
    struct ContactTestResultState
    {
        std::string to_string(const std::string& _each_line_prefix = "\t") const;
        bool operator==(const ContactTestResultState& rhs) const
        {
            return this->to_string() == rhs.to_string();
        }
        std::string                     status_handle;
        Nullable<std::string>           error_msg;
        boost::posix_time::ptime        local_update_time;
        Nullable<unsigned long long>    logd_request_id;
    };

    struct ContactTestResultData
    {
        std::string to_string(const std::string& _each_line_prefix = "\t") const;
        bool operator==(const ContactTestResultData& rhs) const
        {
            return this->to_string() == rhs.to_string();
        }
        std::string                         test_handle;
        boost::posix_time::ptime            local_create_time;
        std::vector<ContactTestResultState> state_history;  /* current state is also included */
    };

    struct ContactCheckState
    {
        std::string to_string(const std::string& _each_line_prefix = "\t") const;
        bool operator==(const ContactCheckState& rhs) const
        {
            return this->to_string() == rhs.to_string();
        }
        std::string                     status_handle;
        boost::posix_time::ptime        local_update_time;
        Nullable<unsigned long long>    logd_request_id;
    };

    std::string to_string(const std::string& _each_line_prefix = "\t") const;

    bool operator==(const InfoContactCheckOutput& rhs) const
    {
        return this->to_string() == rhs.to_string();
    }

    std::string                        handle;
    std::string                        testsuite_handle;
    unsigned long long                 contact_history_id;
    boost::posix_time::ptime           local_create_time;
    std::vector<ContactCheckState>     check_state_history; ///< current state is also included */
    std::vector<ContactTestResultData> tests;
};

/**
 * Get info from existing record in contact_check table. Has no sideeffects.
 */
class InfoContactCheck : public Util::Printable<InfoContactCheck>
{
public:
    /**
     * constructor with only parameter
     * @param _handle     identifies which contact_check to update by it's handle.
     */
    InfoContactCheck(const uuid& _handle);

    /**
     * commit operation
     * @param _output_timezone Postgres time zone input type (as string e. g. "Europe/Prague") for conversion to local time values.
     * @throws LibFred::ExceptionUnknownCheckHandle
     * @return Data of existing check in InfoContactCheckOutput structure.
     */
    InfoContactCheckOutput exec(const OperationContext& _ctx, const std::string& _output_timezone = "Europe/Prague");

    std::string to_string()const;
private:
    /**
     * Get data for tests of specific check.
     * @param _check_id     specifies check which tests data should be retrieved
     */
    static std::vector<InfoContactCheckOutput::ContactTestResultData> get_test_data(
            const OperationContext& _ctx,
            unsigned long long _check_id,
            const std::string& _output_timezone = "Europe/Prague");
    /**
     * Get data for historical states (explicitly: except the current state) of specific check.
     * @param _check_id     specifies check which history should be retrieved
     */
    static std::vector<InfoContactCheckOutput::ContactCheckState> get_check_historical_states(
            const OperationContext& _ctx,
            unsigned long long _check_id,
            const std::string& _output_timezone = "Europe/Prague");

    uuid handle_;
};

}//namespace LibFred

#endif//INFO_CHECK_HH_78A1015DB35948E7AFA58CF421DE454F
