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
 *  nsset info
 */

#ifndef INFO_NSSET_HH_5DB73EDDB0DA4717851421430B3F21F5
#define INFO_NSSET_HH_5DB73EDDB0DA4717851421430B3F21F5

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrable_object/nsset/info_nsset_output.hh"

#include "util/printable.hh"

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>
#include <vector>
#include <utility>

namespace LibFred {

/**
* Nsset info by nsset handle.
* Nsset handle to get info about is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetByHandle : public Util::Printable<InfoNssetByHandle>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_handle, std::string);/**< exception members for unknown nsset handle generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_handle<Exception>
    {};

    /**
    * Info nsset constructor with mandatory parameter.
    * @param handle sets nsset handle into @ref handle_ attribute
    */
    InfoNssetByHandle(const std::string& handle);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetByHandle& set_lock();

    /**
    * Executes getting info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the nsset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoNssetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string handle_;/**< nsset handle */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Nsset info by id.
* Nsset id to get info about the nsset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetById : public Util::Printable<InfoNssetById>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_id, unsigned long long);/**< exception members for unknown object id of the nsset generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_id<Exception>
    {};

    /**
    * Info nsset constructor with mandatory parameter.
    * @param id sets object id of the nsset into @ref id_ attribute
    */
    explicit InfoNssetById(unsigned long long id);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetById& set_lock();

    /**
    * Executes getting info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the nsset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoNssetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long id_;/**< object id of the nsset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Nsset info by DNS server fqdn.
* Name server fqdn is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetByDNSFqdn : public Util::Printable<InfoNssetByDNSFqdn>
{
public:
    /**
    * Info nsset constructor with mandatory parameter.
    * @param fqdn sets name server fqdn into @ref dns_fqdn_ attribute
    */
    InfoNssetByDNSFqdn(const std::string& dns_fqdn);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetByDNSFqdn& set_lock();

    /**
    * Sets limit on number of returned InfoNssetOutput structures.
    * Filter query ordered by nssetid.
    * If not set, there is no limit.
    * Sets  @ref limit_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetByDNSFqdn& set_limit(unsigned long long limit);

    /**
    * Executes getting info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the nssets
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    */
    std::vector<InfoNssetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string dns_fqdn_;/**< name server fqdn*/
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
    Optional<unsigned long long> limit_;/**< max number of returned InfoNssetOutput structures */
};

/**
* Nsset info by tech contact handle.
* Tech contact handle is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetByTechContactHandle : public Util::Printable<InfoNssetByTechContactHandle>
{
public:
    /**
    * Info nsset constructor with mandatory parameter.
    * @param tc_handle sets tech contact handle into @ref tech_contact_handle_ attribute
    */
    InfoNssetByTechContactHandle(const std::string& tc_handle);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetByTechContactHandle& set_lock();

    /**
    * Sets limit on number of returned InfoNssetOutput structures.
    * Filter query ordered by nssetid.
    * If not set, there is no limit.
    * Sets  @ref limit_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetByTechContactHandle& set_limit(unsigned long long limit);

    /**
    * Executes getting info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return info data about the nssets
    */
    std::vector<InfoNssetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string tech_contact_handle_;/**< tech contact handle*/
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
    Optional<unsigned long long> limit_;/**< max number of returned InfoNssetOutput structures */
};

/**
* Nsset history info.
* Output data are arranged in descending order by historyid.
* Nsset registry object identifier to get history info about the nsset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetHistoryByRoid : public Util::Printable<InfoNssetHistoryByRoid>
{
public:
    /**
    * Info nsset history constructor with mandatory parameter.
    * @param roid sets registry object identifier of the nsset into @ref roid_ attribute
    */
    InfoNssetHistoryByRoid(const std::string& roid);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetHistoryByRoid& set_lock();

    /**
    * Executes getting history info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the nsset in descending order by historyid
    */
    std::vector<InfoNssetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const std::string roid_;/**< registry object identifier of the nsset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Nsset info by id including history.
* Output data are arranged in descending order by historyid.
* Nsset id to get info about the nsset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetHistoryById : public Util::Printable<InfoNssetHistoryById>
{
public:
    /**
    * Info nsset history constructor with mandatory parameter.
    * @param id sets object id of the nsset into @ref id_ attribute
    */
    explicit InfoNssetHistoryById(unsigned long long id);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetHistoryById& set_lock();

    /**
    * Executes getting history info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the nsset in descending order by historyid
    */
    std::vector<InfoNssetOutput> exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long id_;/**< object id of the nsset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

/**
* Nsset info by historyid.
* Nsset historyid to get info about the nsset is set via constructor.
* It's executed by @ref exec method with database connection supplied in @ref OperationContext parameter.
*/
class InfoNssetHistoryByHistoryid : public Util::Printable<InfoNssetHistoryByHistoryid>
{
public:
    DECLARE_EXCEPTION_DATA(unknown_object_historyid, unsigned long long);/**< exception members for unknown object historyid of the nsset generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
    , ExceptionData_unknown_object_historyid<Exception>
    {};

    /**
    * Info nsset history constructor with mandatory parameter.
    * @param historyid sets object historyid of the nsset into @ref historyid_ attribute
    */
    explicit InfoNssetHistoryByHistoryid(unsigned long long historyid);

    /**
    * Sets lock for update.
    * Default, if not set, is lock for share.
    * Sets true to lock flag in @ref lock_ attribute
    * @return operation instance reference to allow method chaining
    */
    InfoNssetHistoryByHistoryid& set_lock();

    /**
    * Executes getting history info about the nsset.
    * @param ctx contains reference to database and logging interface
    * @param local_timestamp_pg_time_zone_name is postgresql time zone name of the returned data
    * @return history info data about the nsset
    * @throws Exception in case of wrong input data or other predictable and superable failure.
    * @throws InternalError otherwise
    */
    InfoNssetOutput exec(OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name = "Europe/Prague");

    /**
    * Dumps state of the instance into the string
    * @return string with description of the instance state
    */
    std::string to_string()const;
private:
    const unsigned long long historyid_;/**< history id of the nsset */
    bool lock_;/**< if set to true lock object_registry row for update, if set to false lock for share */
};

}//namespace LibFred

#endif//INFO_NSSET_HH_5DB73EDDB0DA4717851421430B3F21F5
