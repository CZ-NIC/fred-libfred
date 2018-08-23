/*
 * Copyright (C) 2013  CZ.NIC, z.s.p.o.
 *
 * This file is part of FRED.
 *
 * FRED is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License.
 *
 * FRED is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FRED.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 *  @file
 *  enum check status
 */

#ifndef ENUM_CHECK_STATUS_HH_531FE5928B7C498E8904ECE61BD94BAD
#define ENUM_CHECK_STATUS_HH_531FE5928B7C498E8904ECE61BD94BAD

#include <string>
#include <vector>

namespace LibFred {

/**
 * Available statuses for Contact check.
 * Should be in sync with enum_contact_check_status.handle in db.
 */
namespace ContactCheckStatus {

static const std::string ENQUEUE_REQ = "enqueue_req";
static const std::string ENQUEUED = "enqueued";
static const std::string RUNNING = "running";
static const std::string AUTO_TO_BE_DECIDED = "auto_to_be_decided";
static const std::string AUTO_OK = "auto_ok";
static const std::string AUTO_FAIL = "auto_fail";
static const std::string OK = "ok";
static const std::string FAIL_REQ = "fail_req";
static const std::string FAIL = "fail";
static const std::string INVALIDATED = "invalidated";

/**
 * Check statuses sorted into semantical groups
 */

inline std::vector<std::string> get_resolution_awaiting()
{
    return { AUTO_OK,
             AUTO_FAIL,
             AUTO_TO_BE_DECIDED,
             FAIL_REQ,
             ENQUEUE_REQ };  // XXX hack - ugly (but simple) way to enable rejection of ENQUEUE_REQ
}

inline std::vector<std::string> get_after_tests_finished()
{
    return { AUTO_OK,
             AUTO_FAIL,
             AUTO_TO_BE_DECIDED };
}

inline std::vector<std::string> get_tests_updateable()
{
    return { AUTO_OK,
             AUTO_FAIL,
             AUTO_TO_BE_DECIDED,
             FAIL_REQ };
}

inline std::vector<std::string> get_not_yet_resolved()
{
    return { ENQUEUE_REQ,
             ENQUEUED,
             RUNNING,
             AUTO_OK,
             AUTO_FAIL,
             AUTO_TO_BE_DECIDED,
             FAIL_REQ };
}

inline std::vector<std::string> get_possible_resolutions()
{
    return { OK,
             FAIL_REQ,
             FAIL,
             INVALIDATED };
}

inline std::vector<std::string> get_all()
{
    return { ENQUEUE_REQ,
             ENQUEUED,
             RUNNING,
             AUTO_OK,
             AUTO_FAIL,
             AUTO_TO_BE_DECIDED,
             OK,
             FAIL_REQ,
             FAIL,
             INVALIDATED };
}

}//namespace LibFred::ContactCheckStatus
}//namespace LibFred

#endif//ENUM_CHECK_STATUS_HH_531FE5928B7C498E8904ECE61BD94BAD
