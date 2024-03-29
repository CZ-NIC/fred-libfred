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
 *  nsset info
 */

#include <string>
#include <vector>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset_impl.hh"
#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/registrable_object/domain/check_domain.hh"
#include "libfred/registrable_object/contact/check_contact.hh"
#include "util/util.hh"

namespace LibFred
{

    InfoNssetByHandle::InfoNssetByHandle(const std::string& handle)
        : handle_(handle)
        , lock_(false)
    {}

    InfoNssetByHandle& InfoNssetByHandle::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoNssetOutput InfoNssetByHandle::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_res;

        try
        {
            InfoNsset in;
            in.set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::handle())(" = UPPER(").param_text(handle_)(")"))
                .set_history_query(false);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_res = in.exec(ctx, local_timestamp_pg_time_zone_name);

            if (nsset_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_handle(handle_));
            }

            if (nsset_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_res.at(0);
    }

    std::string InfoNssetByHandle::to_string() const
    {
        return Util::format_operation_state("InfoNssetByHandle",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("handle", handle_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoNssetById::InfoNssetById(unsigned long long id)
        : id_(id)
        , lock_(false)
    {}

    InfoNssetById& InfoNssetById::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoNssetOutput InfoNssetById::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_res;

        try
        {
            InfoNsset in;
            in.set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::id())(" = ").param_bigint(id_))
                .set_history_query(false);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_res = in.exec(ctx, local_timestamp_pg_time_zone_name);

            if (nsset_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_object_id(id_));
            }

            if (nsset_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_res.at(0);
    }

    std::string InfoNssetById::to_string() const
    {
        return Util::format_operation_state("InfoNssetById",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("id", boost::lexical_cast<std::string>(id_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

InfoNssetByUuid::InfoNssetByUuid(const RegistrableObject::Nsset::NssetUuid& uuid)
    : uuid_(uuid)
{
}

template <DbLock lock>
InfoNssetOutput InfoNssetByUuid::exec(const OperationContextUsing<lock>& ctx)
{
    try
    {
        InfoNsset ic;
        ic.set_inline_view_filter(
                Database::ParamQuery(InfoNsset::GetAlias::uuid())("=").param_uuid(uuid_))
          .set_history_query(false);

        if (lock == DbLock::for_update)
        {
            ic.set_lock();
        }

        const std::vector<InfoNssetOutput> nsset_res = ic.exec(ctx, "UTC");

        if (nsset_res.empty())
        {
            BOOST_THROW_EXCEPTION(Exception().set_unknown_nsset_uuid(get_raw_value_from(uuid_)));
        }

        if (1 < nsset_res.size())
        {
            BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
        }
        return nsset_res.at(0);
    }
    catch (ExceptionStack& e)
    {
        e.add_exception_stack_info(this->to_string());
        throw;
    }
}

template InfoNssetOutput InfoNssetByUuid::exec<DbLock::for_share>(const OperationContextUsing<DbLock::for_share>&);
template InfoNssetOutput InfoNssetByUuid::exec<DbLock::for_update>(const OperationContextUsing<DbLock::for_update>&);

std::string InfoNssetByUuid::to_string() const
{
    return Util::format_operation_state(
            "InfoNssetByUuid",
            Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("uuid", Util::strong_to_string(uuid_))));
}

InfoNssetByHistoryUuid::InfoNssetByHistoryUuid(const RegistrableObject::Nsset::NssetHistoryUuid& history_uuid)
    : history_uuid_(history_uuid)
{
}

template <DbLock lock>
InfoNssetOutput InfoNssetByHistoryUuid::exec(const OperationContextUsing<lock>& ctx)
{
    try
    {
        InfoNsset ic;
        ic.set_inline_view_filter(
                Database::ParamQuery(InfoNsset::GetAlias::history_uuid())("=").param_uuid(history_uuid_))
          .set_history_query(true);

        if (lock == DbLock::for_update)
        {
            ic.set_lock();
        }

        const std::vector<InfoNssetOutput> nsset_res = ic.exec(ctx, "UTC");

        if (nsset_res.empty())
        {
            BOOST_THROW_EXCEPTION(Exception().set_unknown_nsset_history_uuid(get_raw_value_from(history_uuid_)));
        }

        if (1 < nsset_res.size())
        {
            BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
        }
        return nsset_res.at(0);
    }
    catch (ExceptionStack& e)
    {
        e.add_exception_stack_info(this->to_string());
        throw;
    }
}

template InfoNssetOutput InfoNssetByHistoryUuid::exec<DbLock::for_share>(const OperationContextUsing<DbLock::for_share>&);
template InfoNssetOutput InfoNssetByHistoryUuid::exec<DbLock::for_update>(const OperationContextUsing<DbLock::for_update>&);

std::string InfoNssetByHistoryUuid::to_string() const
{
    return Util::format_operation_state(
            "InfoNssetByHistoryUuid",
            Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("history_uuid", Util::strong_to_string(history_uuid_))));
}

    InfoNssetByDNSFqdn::InfoNssetByDNSFqdn(const std::string& dns_fqdn)
        : dns_fqdn_(dns_fqdn)
        , lock_(false)
    {}

    InfoNssetByDNSFqdn& InfoNssetByDNSFqdn::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoNssetByDNSFqdn& InfoNssetByDNSFqdn::set_limit(unsigned long long limit)
    {
        limit_ = Optional<unsigned long long>(limit);
        return *this;
    }

    std::vector<InfoNssetOutput> InfoNssetByDNSFqdn::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_res;

        try
        {
            Database::ParamQuery cte_id_filter_query;

            cte_id_filter_query("SELECT DISTINCT nssetid FROM host WHERE fqdn = ").param_text(dns_fqdn_);

            if (limit_.isset())
            {
                cte_id_filter_query (" ORDER BY nssetid LIMIT ").param_bigint(limit_.get_value());
            }

            if (ctx.get_conn().exec_params(cte_id_filter_query).size() == 0)
            {
                return nsset_res;
            }

            InfoNsset in;
            in.set_cte_id_filter(cte_id_filter_query)
                .set_history_query(false);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_res = in.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_res;
    }

    std::string InfoNssetByDNSFqdn::to_string() const
    {
        return Util::format_operation_state("InfoNssetByDNSFqdn",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("dns_fqdn", dns_fqdn_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        (std::make_pair("limit", limit_.print_quoted()))
        );
    }


    InfoNssetByTechContactHandle::InfoNssetByTechContactHandle(const std::string& tc_handle)
        : tech_contact_handle_(tc_handle)
        , lock_(false)
    {}

    InfoNssetByTechContactHandle& InfoNssetByTechContactHandle::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoNssetByTechContactHandle& InfoNssetByTechContactHandle::set_limit(unsigned long long limit)
    {
        limit_ = Optional<unsigned long long>(limit);
        return *this;
    }

    std::vector<InfoNssetOutput> InfoNssetByTechContactHandle::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_res;

        try
        {
            Database::ParamQuery cte_id_filter_query;

            cte_id_filter_query(
                "SELECT DISTINCT ncm.nssetid"
                    " FROM object_registry oreg"
                        " JOIN  enum_object_type eot ON oreg.type = eot.id AND eot.name = 'contact'"
                        " JOIN nsset_contact_map ncm ON ncm.contactid = oreg.id"
                    " WHERE oreg.name = UPPER(").param_text(tech_contact_handle_)(") AND oreg.erdate IS NULL");

            if (limit_.isset())
            {
                cte_id_filter_query (" ORDER BY ncm.nssetid LIMIT ").param_bigint(limit_.get_value());
            }

            if (ctx.get_conn().exec_params(cte_id_filter_query).size() == 0)
            {
                return nsset_res;
            }

            InfoNsset in;
            in.set_cte_id_filter(cte_id_filter_query)
                .set_history_query(false);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_res = in.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_res;
    }


    std::string InfoNssetByTechContactHandle::to_string() const
    {
        return Util::format_operation_state("InfoNssetByTechContactHandle",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("tech_contact_handle", tech_contact_handle_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        (std::make_pair("limit", limit_.print_quoted()))
        );
    }


    InfoNssetHistoryByRoid::InfoNssetHistoryByRoid(const std::string& roid)
    : roid_(roid)
    , lock_(false)
    {}

    InfoNssetHistoryByRoid& InfoNssetHistoryByRoid::set_lock()
    {
        lock_ = true;
        return *this;
    }

    std::vector<InfoNssetOutput> InfoNssetHistoryByRoid::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_res;

        try
        {
            InfoNsset in;
            in.set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::roid())(" = ").param_text(roid_))
                .set_history_query(true);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_res = in.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_res;
    }

    std::string InfoNssetHistoryByRoid::to_string() const
    {
        return Util::format_operation_state("InfoNssetHistoryByRoid",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("roid", roid_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }


    InfoNssetHistoryById::InfoNssetHistoryById(unsigned long long id)
        : id_(id)
        , lock_(false)
    {}

    InfoNssetHistoryById& InfoNssetHistoryById::set_lock()
    {
        lock_ = true;
        return *this;
    }

    std::vector<InfoNssetOutput> InfoNssetHistoryById::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_history_res;

        try
        {
            InfoNsset in;
            in.set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::id())(" = ").param_bigint(id_))
                .set_history_query(true);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_history_res = in.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_history_res;
    }

    std::string InfoNssetHistoryById::to_string() const
    {
        return Util::format_operation_state("InfoNssetHistoryById",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("id", boost::lexical_cast<std::string>(id_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoNssetHistoryByHistoryid::InfoNssetHistoryByHistoryid(unsigned long long historyid)
        : historyid_(historyid)
        , lock_(false)
    {}

    InfoNssetHistoryByHistoryid& InfoNssetHistoryByHistoryid::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoNssetOutput InfoNssetHistoryByHistoryid::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoNssetOutput> nsset_history_res;

        try
        {
            InfoNsset in;
            in.set_inline_view_filter(Database::ParamQuery(InfoNsset::GetAlias::historyid())(" = ").param_bigint(historyid_))
                .set_history_query(true);
            if (lock_)
            {
                in.set_lock();
            }
            nsset_history_res = in.exec(ctx, local_timestamp_pg_time_zone_name);

            if (nsset_history_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_object_historyid(historyid_));
            }

            if (nsset_history_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return nsset_history_res.at(0);
    }

    std::string InfoNssetHistoryByHistoryid::to_string() const
    {
        return Util::format_operation_state("InfoNssetHistoryByHistoryid",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("historyid", boost::lexical_cast<std::string>(historyid_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

} // namespace LibFred

