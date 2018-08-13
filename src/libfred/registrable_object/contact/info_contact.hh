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
 *  contact info
 */

#ifndef INFO_CONTACT_HH_E0B2A756ECD64B59AA1679BD8ECE51C0
#define INFO_CONTACT_HH_E0B2A756ECD64B59AA1679BD8ECE51C0

#include "src/libfred/opexception.hh"
#include "src/libfred/opcontext.hh"
#include "src/util/optional_value.hh"
#include "src/util/printable.hh"
#include "src/libfred/registrable_object/contact/info_contact_output.hh"
#include "src/libfred/registrable_object/contact/place_address.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
* Contact info by handle.
* Contact handle to get info about the contact is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoContactByHandle : public Util::Printable
{
public:
    DECLARE_EXCEPTION_DATA(unknown_contact_handle, std::string);/**< exception members for unknown handle of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_contact_handle<Exception>
    {};

    /**
    * Info contact constructor with mandatory parameter.
    * @param handle sets handle of the contact into @ref handle_ attribute
    */
    InfoContactByHandle(const std::string& handle);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoContactByHandle& set_lock();

    /**
    * Executes getting info about the contact.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the contact
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoContactOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;
private:
    const std::string handle_;/**< handle of the contact */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Contact info by id.
* Contact id to get info about the contact is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoContactById : public Util::Printable
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_id, unsigned long long);/**< exception members for unknown object id of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_id<Exception>
    {};

    /**
    * Info contact constructor with mandatory parameter.
    * @param id sets object id of the contact into @ref id_ attribute
    */
    explicit InfoContactById(unsigned long long id);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoContactById& set_lock();

    /**
    * Executes getting info about the contact.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the contact
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoContactOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;
private:
    const unsigned long long id_;/**< object id of the contact */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Contact history info by registry object identifier  and optional time.
* Output data are arranged in descending order by historyid.
* Contact registry object identifier to get history info about the contact is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoContactHistoryByRoid  : public Util::Printable
{
public:
    DECLARE_EXCEPTION_DATA(unknown_registry_object_identifier, std::string);/**< exception members for unknown registry object identifier of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_registry_object_identifier<Exception>
    {};

    /**
    * Info contact history constructor with mandatory parameter.
    * @param roid sets registry object identifier of the contact into @ref roid_ attribute
    */
    InfoContactHistoryByRoid(const std::string& roid);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoContactHistoryByRoid& set_lock();

    /**
    * Executes getting history info about the contact.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the contact in descending order by historyid
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    std::vector<InfoContactOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;
private:
    const std::string roid_;/**< registry object identifier of the contact */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Contact info by id including history.
* Output data are arranged in descending order by historyid.
* Contact id to get info about the contact is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoContactHistoryById : public Util::Printable
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_id, unsigned long long);/**< exception members for unknown object id of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_id<Exception>
    {};

    /**
    * Info contact history constructor with mandatory parameter.
    * @param id sets object id of the contact into @ref id_ attribute
    */
    explicit InfoContactHistoryById(unsigned long long id);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoContactHistoryById& set_lock();

    /**
    * Executes getting history info about the contact.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the contact in descending order by historyid
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    std::vector<InfoContactOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;
private:
    const unsigned long long id_;/**< object id of the contact */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Contact info by historyid.
* Contact historyid to get info about the contact is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoContactHistoryByHistoryid : public Util::Printable
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_historyid, unsigned long long);/**< exception members for unknown object historyid of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_historyid<Exception>
    {};

    /**
    * Info contact history constructor with mandatory parameter.
    * @param historyid sets object historyid of the contact into @ref historyid_ attribute
    */
    explicit InfoContactHistoryByHistoryid(unsigned long long historyid);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoContactHistoryByHistoryid& set_lock();

    /**
    * Executes getting history info about the contact.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the contact
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoContactOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string() const;
private:
    const unsigned long long historyid_;/**< history id of the contact */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

}//namespace LibFred

#endif//INFO_CONTACT_HH_E0B2A756ECD64B59AA1679BD8ECE51C0
