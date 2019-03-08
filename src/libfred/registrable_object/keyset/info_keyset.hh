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
 *  keyset info
 */

#ifndef INFO_KEYSET_HH_862A5CBB5F5F4D1BB177D463C11E8BD1
#define INFO_KEYSET_HH_862A5CBB5F5F4D1BB177D463C11E8BD1

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrable_object/keyset/info_keyset_output.hh"

#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>

#include <string>
#include <vector>

namespace LibFred {

/**
* Keyset info by keyset handle.
* Keyset handle to get info about is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetByHandle : public Util::Printable<InfoKeysetByHandle>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_handle, std::string);/**< exception members for unknown keyset handle generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_handle<Exception>
    {};

    /**
    * Info keyset constructor with mandatory parameter.
    * @param handle sets keyset handle into @ref handle_ attribute
    */
    InfoKeysetByHandle(const std::string& handle);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeysetByHandle& set_lock();

    /**
    * Executes getting info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the keyset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoKeysetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string handle_;/**< keyset handle */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Keyset info by id.
* Keyset id to get info about the keyset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetById : public Util::Printable<InfoKeysetById>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_id, unsigned long long);/**< exception members for unknown object id of the keyset generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_id<Exception>
    {};

    /**
    * Info keyset constructor with mandatory parameter.
    * @param id sets object id of the keyset into @ref id_ attribute
    */
    explicit InfoKeysetById(unsigned long long id);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeysetById& set_lock();

    /**
    * Executes getting info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the keyset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoKeysetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long id_;/**< object id of the keyset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Keyset history info.
* Output data are arranged in descending order by historyid.
* Keyset registry object identifier to get history info about the keyset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetHistoryByRoid : public Util::Printable<InfoKeysetHistoryByRoid>
{
public:
    /**
    * Info keyset history constructor with mandatory parameter.
    * @param roid sets registry object identifier of the keyset into @ref roid_ attribute
    */
    InfoKeysetHistoryByRoid(const std::string& roid);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeysetHistoryByRoid& set_lock();

    /**
    * Executes getting history info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the keyset in descending order by historyid
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    std::vector<InfoKeysetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string roid_;/**< registry object identifier of the keyset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Keyset info by id including history.
* Output data are arranged in descending order by historyid.
* Keyset id to get info about the keyset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetHistoryById : public Util::Printable<InfoKeysetHistoryById>
{
public:
    /**
    * Info keyset history constructor with mandatory parameter.
    * @param id sets object id of the keyset into @ref id_ attribute
    */
    explicit InfoKeysetHistoryById(unsigned long long id);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeysetHistoryById& set_lock();

    /**
    * Executes getting history info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the keyset in descending order by historyid
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    std::vector<InfoKeysetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long id_;/**< object id of the keyset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Keyset info by historyid.
* Keyset historyid to get info about the keyset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetHistoryByHistoryid : public Util::Printable<InfoKeysetHistoryByHistoryid>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_historyid, unsigned long long);/**< exception members for unknown object historyid of the keyset generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_historyid<Exception>
    {};

    /**
    * Info keyset history constructor with mandatory parameter.
    * @param historyid sets object historyid of the keyset into @ref historyid_ attribute
    */
    explicit InfoKeysetHistoryByHistoryid(unsigned long long historyid);

    /**
     * Sets lock for update.
     * Default, if not set, is lock for share.
     * Sets true to lock flag in @ref lock_ attribute
     * @return operation instance reference to allow method chaining
     */
    InfoKeysetHistoryByHistoryid& set_lock();

    /**
    * Executes getting history info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the keyset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoKeysetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long historyid_;/**< history id of the keyset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Keyset info by tech contact handle.
* Tech contact handle is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoKeysetByTechContactHandle : public Util::Printable<InfoKeysetByTechContactHandle>
{
public:
    /**
    * Info keyset constructor with mandatory parameter.
    * @param tc_handle sets tech contact handle into @ref tech_contact_handle_ attribute
    */
    InfoKeysetByTechContactHandle(const std::string& tc_handle);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoKeysetByTechContactHandle& set_lock();

    /**
    * Sets limit on number of returned InfoKeysetOutput structures.
    * Filter query ordered by keysetid.
    * If not set, there is no limit.
    * Sets  @ref limit_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoKeysetByTechContactHandle& set_limit(unsigned long long limit);


    /**
    * Executes getting info about the keyset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the keysets
    * @throws InternalError
    */
    std::vector<InfoKeysetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string tech_contact_handle_;/**< tech contact handle*/
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
    Optional<unsigned long long> limit_;/**< max number of returned InfoKeysetOutput structures */
};

}//namespace LibFred

#endif//INFO_KEYSET_HH_862A5CBB5F5F4D1BB177D463C11E8BD1
