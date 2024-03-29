/*
 * Copyright (C) 2018-2022  CZ.NIC, z. s. p. o.
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
#include "test/setup/fixtures_utils.hh"

#include <cmath>

namespace Test {

unsigned long long generate_random_bigserial()
{
    return Random::Generator().get(0, std::numeric_limits<int>::max());
}

std::string generate_random_handle()
{
    return Random::Generator().get_seq(Random::CharSet::digits(), 20);
}

unsigned long long get_nonexistent_object_id(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "object_registry", "id", "bigint", generate_random_bigserial);
}

unsigned long long get_nonexistent_object_historyid(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "object_history", "historyid", "bigint", generate_random_bigserial);
}

std::string get_nonexistent_object_handle(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "object_registry", "name", "text", generate_random_handle);
}

unsigned long long get_nonexistent_message_id(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "message", "id", "bigint", generate_random_bigserial);
}

unsigned long long get_nonexistent_registrar_id(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "registrar", "id", "bigint", generate_random_bigserial);
}

unsigned long long  get_nonexistent_zone_id(const ::LibFred::OperationContext& ctx)
{
    return get_nonexistent_value(ctx, "zone", "id", "bigint", generate_random_bigserial);
}

unsigned long long get_cz_zone_id(const ::LibFred::OperationContext& ctx)
{
    return static_cast<unsigned long long>(ctx.get_conn().exec("SELECT id FROM zone WHERE fqdn='cz'")[0][0]);
}

add_admin_contacts_to_domain::add_admin_contacts_to_domain(
        const std::string& _domain_handle,
        const std::string& _registrar_handle)
    : domain_handle_(_domain_handle),
      registrar_handle_(_registrar_handle)
{ }

std::pair<std::string, std::vector<std::string>> add_admin_contacts_to_domain::exec(const ::LibFred::OperationContext& ctx)
{
    ::LibFred::UpdateDomain update_domain(domain_handle_, registrar_handle_);
    for (const auto& c : values_)
    {
        update_domain.add_admin_contact(c);
    }
    update_domain.exec(ctx);
    return std::make_pair(domain_handle_, values_);
}

add_admin_contact_to_domains::add_admin_contact_to_domains(
        const std::string& _contact_handle,
        const std::string& _registrar_handle)
    : contact_handle_(_contact_handle),
      registrar_handle_(_registrar_handle)
{ }

std::pair<std::string, std::vector<std::string>> add_admin_contact_to_domains::exec(const ::LibFred::OperationContext& ctx)
{
    for (const auto& d : values_)
    {
        ::LibFred::UpdateDomain(d, registrar_handle_).add_admin_contact(contact_handle_).exec(ctx);
    }
    return std::make_pair(contact_handle_, values_);
}

}//namespace Test
