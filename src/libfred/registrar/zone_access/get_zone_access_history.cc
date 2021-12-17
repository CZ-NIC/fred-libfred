/*
 * Copyright (C) 2019-2021  CZ.NIC, z. s. p. o.
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
#include "libfred/db_settings.hh"
#include "libfred/registrar/zone_access/exceptions.hh"
#include "libfred/registrar/zone_access/get_zone_access_history.hh"
#include "util/db/query_param.hh"

namespace LibFred {
namespace Registrar {
namespace ZoneAccess {

GetZoneAccessHistory::GetZoneAccessHistory(const std::string& _registrar_handle)
    : registrar_handle_(_registrar_handle)
{
}

RegistrarZoneAccessHistory GetZoneAccessHistory::exec(OperationContext& _ctx)const
{
    try
    {
        constexpr auto column_zone_fqdn = 0;
        constexpr auto column_registrar_id = 1;
        constexpr auto column_invoice_id = 2;
        constexpr auto column_invoice_from_date = 3;
        constexpr auto column_invoice_to_date = 4;
        const Database::Result db_result = _ctx.get_conn().exec_params(
                "SELECT z.fqdn,r.id,ri.id,ri.fromdate,ri.todate "
                "FROM zone z "
                "LEFT JOIN registrar r ON r.handle=UPPER($1::TEXT) "
                "LEFT JOIN registrarinvoice ri ON ri.zone=z.id AND ri.registrarid=r.id",
                Database::QueryParams({registrar_handle_}));

        if (0 < db_result.size())
        {
            if (db_result[0][column_registrar_id].isnull())
            {
                struct RegistrarDoesNotExist : NonexistentRegistrar, Exception
                {
                    const char* what() const noexcept override
                    {
                        return "registrar does not exist";
                    }
                };
                FREDLOG_DEBUG("registrar \"" + registrar_handle_ + "\" does not exist");
                throw RegistrarDoesNotExist();
            }
        }
        RegistrarZoneAccessHistory retval;
        retval.registrar_handle = registrar_handle_;
        for (std::size_t idx = 0; idx < db_result.size(); ++idx)
        {
            if (!db_result[idx][column_invoice_id].isnull())
            {
                TimeInterval invoice_time;
                const auto invoice_id = db_result[idx][column_invoice_id].as<RegistrarInvoiceId>();
                const auto zone_fqdn = static_cast<std::string>(db_result[idx][column_zone_fqdn]);
                if (!db_result[idx][column_invoice_from_date].isnull())
                {
                    invoice_time.from_date = boost::gregorian::from_string(
                            static_cast<std::string>(db_result[idx][column_invoice_from_date]));
                }
                if (!db_result[idx][column_invoice_to_date].isnull())
                {
                    invoice_time.to_date = boost::gregorian::from_string(
                            static_cast<std::string>(db_result[idx][column_invoice_to_date]));
                }
                const bool successfully_inserted = retval.invoices_by_zone[zone_fqdn].insert({invoice_time, invoice_id}).second;
                if (!successfully_inserted)
                {
                    struct InvoiceInsertionFailure : OverlappingZoneAccessRange, Exception
                    {
                        const char* what() const noexcept override
                        {
                            return "invoice insertion failed";
                        }
                    };
                    FREDLOG_DEBUG("for registrar \"" + registrar_handle_ + "\" in \"" + zone_fqdn + "\" zone "
                                         "access range overlapping detected");
                    throw InvoiceInsertionFailure();
                }
            }
        }
        return retval;
    }
    catch (const NonexistentRegistrar&)
    {
        FREDLOG_INFO("GetZoneAccessHistory::exec() throws NonexistentRegistrar() exception");
        throw;
    }
    catch (const OverlappingZoneAccessRange&)
    {
        FREDLOG_INFO("GetZoneAccessHistory::exec() throws OverlappingZoneAccessRange() exception");
        throw;
    }
    catch (const Exception& e)
    {
        FREDLOG_INFO(std::string("GetZoneAccessHistory::exec() throws Exception(\"") + e.what() + "\")");
        throw;
    }
    catch (const std::exception& e)
    {
        FREDLOG_WARNING(std::string("GetZoneAccessHistory::exec() caught an std::exception(\"") + e.what() + "\")");
        struct UnknownException : Exception
        {
            const char* what() const noexcept override
            {
                return "Failed to get registrar's zone access due to an unknown exception.";
            }
        };
        throw UnknownException();
    }
    catch (...)
    {
        FREDLOG_ERROR("GetZoneAccessHistory::exec() throws an unexpected exception");
        throw;
    }
}

}//namespace LibFred::Registrar::ZoneAccess
}//namespace LibFred::Registrar
}//namespace LibFred
