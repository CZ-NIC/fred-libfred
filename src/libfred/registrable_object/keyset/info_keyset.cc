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
 *  keyset info
 */

#include "libfred/registrable_object/keyset/info_keyset.hh"

#include "libfred/opcontext.hh"
#include "libfred/opexception.hh"
#include "libfred/registrable_object/contact/check_contact.hh"
#include "libfred/registrable_object/keyset/info_keyset_impl.hh"
#include "util/util.hh"

#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_period.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include <string>
#include <vector>

namespace LibFred
{

    InfoKeysetByHandle::InfoKeysetByHandle(const std::string& handle)
        : handle_(handle)
        , lock_(false)
    {}

    InfoKeysetByHandle& InfoKeysetByHandle::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoKeysetOutput InfoKeysetByHandle::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_res;

        try
        {
            InfoKeyset ik;
            ik.set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::handle())(" = UPPER(").param_text(handle_)(")")).set_history_query(false);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);

            if (keyset_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_handle(handle_));
            }

            if (keyset_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_res.at(0);
    }

    std::string InfoKeysetByHandle::to_string() const
    {
        return Util::format_operation_state("InfoKeysetByHandle",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("handle", handle_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoKeysetById::InfoKeysetById(unsigned long long id)
        : id_(id)
        , lock_(false)
    {}

    InfoKeysetById& InfoKeysetById::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoKeysetOutput InfoKeysetById::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_res;

        try
        {
            InfoKeyset ik;
            ik.set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::id())(" = ").param_bigint(id_)).set_history_query(false);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);

            if (keyset_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_object_id(id_));
            }

            if (keyset_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_res.at(0);
    }

    std::string InfoKeysetById::to_string() const
    {
        return Util::format_operation_state("InfoKeysetById",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("id", boost::lexical_cast<std::string>(id_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

InfoKeysetByUuid::InfoKeysetByUuid(const RegistrableObject::Keyset::KeysetUuid& uuid)
    : uuid_(uuid)
{
}

template <DbLock lock>
InfoKeysetOutput InfoKeysetByUuid::exec(const OperationContextUsing<lock>& ctx)
{
    try
    {
        InfoKeyset ic;
        ic.set_inline_view_filter(
                Database::ParamQuery(InfoKeyset::GetAlias::uuid())("=").param_uuid(uuid_))
          .set_history_query(false);

        if (lock == DbLock::for_update)
        {
            ic.set_lock();
        }

        const std::vector<InfoKeysetOutput> keyset_res = ic.exec(ctx, "UTC");

        if (keyset_res.empty())
        {
            BOOST_THROW_EXCEPTION(Exception().set_unknown_keyset_uuid(get_raw_value_from(uuid_)));
        }

        if (1 < keyset_res.size())
        {
            BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
        }
        return keyset_res.at(0);
    }
    catch (ExceptionStack& e)
    {
        e.add_exception_stack_info(this->to_string());
        throw;
    }
}

template InfoKeysetOutput InfoKeysetByUuid::exec<DbLock::for_share>(const OperationContextUsing<DbLock::for_share>&);
template InfoKeysetOutput InfoKeysetByUuid::exec<DbLock::for_update>(const OperationContextUsing<DbLock::for_update>&);

std::string InfoKeysetByUuid::to_string() const
{
    return Util::format_operation_state(
            "InfoKeysetByUuid",
            Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("uuid", Util::strong_to_string(uuid_))));
}

InfoKeysetByHistoryUuid::InfoKeysetByHistoryUuid(const RegistrableObject::Keyset::KeysetHistoryUuid& history_uuid)
    : history_uuid_(history_uuid)
{
}

template <DbLock lock>
InfoKeysetOutput InfoKeysetByHistoryUuid::exec(const OperationContextUsing<lock>& ctx)
{
    try
    {
        InfoKeyset ic;
        ic.set_inline_view_filter(
                Database::ParamQuery(InfoKeyset::GetAlias::history_uuid())("=").param_uuid(history_uuid_))
          .set_history_query(true);

        if (lock == DbLock::for_update)
        {
            ic.set_lock();
        }

        const std::vector<InfoKeysetOutput> keyset_res = ic.exec(ctx, "UTC");

        if (keyset_res.empty())
        {
            BOOST_THROW_EXCEPTION(Exception().set_unknown_keyset_history_uuid(get_raw_value_from(history_uuid_)));
        }

        if (1 < keyset_res.size())
        {
            BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
        }
        return keyset_res.at(0);
    }
    catch (ExceptionStack& e)
    {
        e.add_exception_stack_info(this->to_string());
        throw;
    }
}

template InfoKeysetOutput InfoKeysetByHistoryUuid::exec<DbLock::for_share>(const OperationContextUsing<DbLock::for_share>&);
template InfoKeysetOutput InfoKeysetByHistoryUuid::exec<DbLock::for_update>(const OperationContextUsing<DbLock::for_update>&);

std::string InfoKeysetByHistoryUuid::to_string() const
{
    return Util::format_operation_state(
            "InfoKeysetByHistoryUuid",
            Util::vector_of<std::pair<std::string, std::string>>
                    (std::make_pair("history_uuid", Util::strong_to_string(history_uuid_))));
}


    InfoKeysetHistoryByRoid::InfoKeysetHistoryByRoid(const std::string& roid)
    : roid_(roid)
    , lock_(false)
    {}

    InfoKeysetHistoryByRoid& InfoKeysetHistoryByRoid::set_lock()
    {
        lock_ = true;
        return *this;
    }

    std::vector<InfoKeysetOutput> InfoKeysetHistoryByRoid::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_res;

        try
        {
            InfoKeyset ik;
            ik.set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::roid())(" = ").param_text(roid_)).set_history_query(true);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_res;
    }

    std::string InfoKeysetHistoryByRoid::to_string() const
    {
        return Util::format_operation_state("InfoKeysetHistoryByRoid",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("roid", roid_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }


    InfoKeysetHistoryById::InfoKeysetHistoryById(unsigned long long id)
        : id_(id)
        , lock_(false)
    {}

    InfoKeysetHistoryById& InfoKeysetHistoryById::set_lock()
    {
        lock_ = true;
        return *this;
    }

    std::vector<InfoKeysetOutput> InfoKeysetHistoryById::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_history_res;

        try
        {
            InfoKeyset ik;
            ik.set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::id())(" = ").param_bigint(id_)).set_history_query(true);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_history_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_history_res;
    }

    std::string InfoKeysetHistoryById::to_string() const
    {
        return Util::format_operation_state("InfoKeysetHistoryById",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("id", boost::lexical_cast<std::string>(id_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }

    InfoKeysetHistoryByHistoryid::InfoKeysetHistoryByHistoryid(unsigned long long historyid)
        : historyid_(historyid)
        , lock_(false)
    {}

    InfoKeysetHistoryByHistoryid& InfoKeysetHistoryByHistoryid::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoKeysetOutput InfoKeysetHistoryByHistoryid::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_history_res;

        try
        {
            InfoKeyset ik;
            ik.set_inline_view_filter(Database::ParamQuery(InfoKeyset::GetAlias::historyid())(" = ").param_bigint(historyid_)).set_history_query(true);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_history_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);

            if (keyset_history_res.empty())
            {
                BOOST_THROW_EXCEPTION(Exception().set_unknown_object_historyid(historyid_));
            }

            if (keyset_history_res.size() > 1)
            {
                BOOST_THROW_EXCEPTION(InternalError("query result size > 1"));
            }

        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_history_res.at(0);
    }

    std::string InfoKeysetHistoryByHistoryid::to_string() const
    {
        return Util::format_operation_state("InfoKeysetHistoryByHistoryid",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("historyid", boost::lexical_cast<std::string>(historyid_)))
        (std::make_pair("lock", lock_ ? "true":"false"))
        );
    }


    InfoKeysetByTechContactHandle::InfoKeysetByTechContactHandle(const std::string& tc_handle)
        : tech_contact_handle_(tc_handle)
        , lock_(false)
    {}

    InfoKeysetByTechContactHandle& InfoKeysetByTechContactHandle::set_lock()
    {
        lock_ = true;
        return *this;
    }

    InfoKeysetByTechContactHandle& InfoKeysetByTechContactHandle::set_limit(unsigned long long limit)
    {
        limit_ = Optional<unsigned long long>(limit);
        return *this;
    }

    std::vector<InfoKeysetOutput> InfoKeysetByTechContactHandle::exec(const OperationContext& ctx, const std::string& local_timestamp_pg_time_zone_name)
    {
        std::vector<InfoKeysetOutput> keyset_res;

        try
        {
            Database::ParamQuery cte_id_filter_query;

            cte_id_filter_query(
                "SELECT DISTINCT kcm.keysetid"
                    " FROM object_registry oreg"
                        " JOIN  enum_object_type eot ON oreg.type = eot.id AND eot.name = 'contact'"
                        " JOIN keyset_contact_map kcm ON kcm.contactid = oreg.id"
                    " WHERE oreg.name = UPPER(").param_text(tech_contact_handle_)(") AND oreg.erdate IS NULL");

            if (limit_.isset())
            {
                cte_id_filter_query (" ORDER BY kcm.keysetid LIMIT ").param_bigint(limit_.get_value());
            }

            if (ctx.get_conn().exec_params(cte_id_filter_query).size() == 0)
            {
                return keyset_res;
            }

            InfoKeyset ik;
            ik.set_cte_id_filter(cte_id_filter_query)
                .set_history_query(false);

            if (lock_)
            {
                ik.set_lock();
            }

            keyset_res = ik.exec(ctx, local_timestamp_pg_time_zone_name);
        }
        catch (ExceptionStack& ex)
        {
            ex.add_exception_stack_info(to_string());
            throw;
        }
        return keyset_res;
    }


    std::string InfoKeysetByTechContactHandle::to_string() const
    {
        return Util::format_operation_state("InfoKeysetByTechContactHandle",
        Util::vector_of<std::pair<std::string, std::string> >
        (std::make_pair("tech_contact_handle", tech_contact_handle_))
        (std::make_pair("lock", lock_ ? "true":"false"))
        (std::make_pair("limit", limit_.print_quoted()))
        );
    }


} // namespace LibFred

