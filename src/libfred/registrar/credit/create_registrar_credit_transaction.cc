/*
 * Copyright (C) 2019  CZ.NIC, z.s.p.o.
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

#include "libfred/db_settings.hh"
#include "libfred/registrar/credit/create_registrar_credit_transaction.hh"
#include "libfred/registrar/credit/exceptions.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/zone/exceptions.hh"
#include "libfred/zone/info_zone.hh"
#include "libfred/registrar/zone_access/exceptions.hh"
#include "libfred/registrar/zone_access/get_zone_access_history.hh"
#include "libfred/registrar/zone_access/registrar_zone_access_type.hh"

namespace LibFred {
namespace Registrar {
namespace Credit {

CreateRegistrarCreditTransaction::CreateRegistrarCreditTransaction(
        const std::string& _registrar,
        const std::string& _zone,
        Decimal _credit_change)
    : registrar_(_registrar),
      zone_(_zone),
      credit_change_(_credit_change)
{
}

unsigned long long CreateRegistrarCreditTransaction::exec(OperationContext& _ctx) const
{
    try
    {
        const auto registrar_id =
                LibFred::InfoRegistrarByHandle(registrar_)
                .exec(_ctx).info_registrar_data.id;

        const auto zone_id = LibFred::Zone::get_zone_id(
                LibFred::Zone::InfoZone(zone_).exec(_ctx));

        const auto zone_access_history = LibFred::Registrar::ZoneAccess::GetZoneAccessHistory(registrar_).exec(_ctx);
        if (!LibFred::Registrar::ZoneAccess::has_access(zone_access_history, zone_, boost::gregorian::day_clock::local_day()))
        {
            _ctx.get_log().debug("registrar \"" + registrar_ + "\" has no access to zone \"" + zone_ + "\"");
            throw NonexistentZoneAccess();
        }

        Database::Result registrar_credit_id_result = _ctx.get_conn().exec_params(
                // clang-format off
                "SELECT id "
                  "FROM registrar_credit "
                 "WHERE registrar_id = $1::bigint "
                   "AND zone_id = $2::bigint ",
                // clang-format on
                Database::query_param_list(registrar_id)(zone_id));

        unsigned long long registrar_credit_id = 0;
        if (registrar_credit_id_result.size() == 1)
        {
            registrar_credit_id = static_cast<unsigned long long>(registrar_credit_id_result[0][0]);
        }
        else
        {
            _ctx.get_conn().exec("LOCK TABLE registrar_credit IN ACCESS EXCLUSIVE MODE");
            Database::Result init_credit_result = _ctx.get_conn().exec_params(
                    // clang-format off
                    "WITH cte AS ("
                        "INSERT INTO registrar_credit (credit, registrar_id, zone_id) "
                        "VALUES (0, $1::bigint, $2::bigint) "
                        "ON CONFLICT DO NOTHING "
                        "RETURNING id) "
                    "SELECT id FROM cte "
                     "WHERE EXISTS (SELECT 1 FROM cte) "
                     "UNION ALL "
                    "SELECT rc.id FROM registrar_credit rc "
                     "WHERE NOT EXISTS (SELECT 1 FROM cte) "
                       "AND rc.registrar_id = $1::bigint "
                       "AND rc.zone_id = $2::bigint ",
                    // clang-format on
                    Database::query_param_list(registrar_id)(zone_id));
            if (init_credit_result.size() == 1)
            {
                registrar_credit_id = static_cast<unsigned long long>(init_credit_result[0][0]);
            }
            else
            {
                throw CreateTransactionException();
            }
        }

        Database::Result registrar_credit_transaction_id_result = _ctx.get_conn().exec_params(
                // clang-format off
                 "INSERT INTO registrar_credit_transaction (id, balance_change, registrar_credit_id) "
                 "VALUES (DEFAULT, $1::numeric, $2::bigint) "
                 "RETURNING id",
                // clang-format on
                Database::query_param_list(credit_change_)(registrar_credit_id));

        if (registrar_credit_transaction_id_result.size() == 1)
        {
            const auto registrar_credit_transaction_id =
                    static_cast<unsigned long long>(registrar_credit_transaction_id_result[0][0]);
            return registrar_credit_transaction_id;
        }
    }
    catch (const LibFred::InfoRegistrarByHandle::Exception& e)
    {
        if (e.is_set_unknown_registrar_handle())
        {
            throw NonexistentRegistrar();
        }
        throw;
    }
    catch (const LibFred::Zone::NonExistentZone&)
    {
        throw;
    }
    catch (const LibFred::Registrar::ZoneAccess::GetRegistrarZoneAccessException&)
    {
        throw ZoneAccessException();
    }
    catch (const NonexistentZoneAccess&)
    {
        throw;
    }
    catch (const CreateTransactionException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        LOGGER.info(e.what());
        throw CreateTransactionException();
    }
    throw CreateTransactionException();
}

} // namespace LibFred::Registrar::Credit
} // namespace LibFred::Registrar
} // namespace LibFred
