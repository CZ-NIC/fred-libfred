/*
 * Copyright (C) 2022  CZ.NIC, z. s. p. o.
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

#include "libfred/registrar/certification/delete_registrar_certification.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace LibFred {
namespace Registrar {

DeleteRegistrarCertification::DeleteRegistrarCertification(const RegistrarCertificationIdVariant& _certification_id)
    : certification_id_{_certification_id}
{ }

namespace {

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

class Exec : public boost::static_visitor<RegistrarCertification>
{
public:
    explicit Exec(const OperationContext& ctx)
        : ctx_{ctx}
    { }
    RegistrarCertification operator()(unsigned long long certification_id) const
    {
        const auto dbres = ctx_.get_conn().exec_params(
                "DELETE FROM registrar_certification "
                 "WHERE id = $1::BIGINT "
             "RETURNING id, "
                       "uuid, "
                       "valid_from, "
                       "valid_until, "
                       "classification, "
                       "eval_file_id, "
                       "eval_file_uuid",
                Database::QueryParams{certification_id});
        if (dbres.size() <= 0)
        {
            struct NotFound : RegistrarCertificationDoesNotExist
            {
                const char* what() const noexcept override { return "registrar certification not found by id"; }
            };
            throw NotFound{};
        }
        if (1 < dbres.size())
        {
            struct TooManyCertifications : std::exception
            {
                const char* what() const noexcept override { return "too many registrar certifications found by id"; }
            };
            throw TooManyCertifications{};
        }
        const auto row = dbres[0];
        return RegistrarCertification{
                static_cast<unsigned long long>(row[0]),
                {to_uuid(row[1])},
                boost::gregorian::from_string(static_cast<std::string>(row[2])),
                row[3].isnull() ? boost::gregorian::date{boost::date_time::special_values::pos_infin}
                                : boost::gregorian::from_string(static_cast<std::string>(row[3])),
                static_cast<int>(row[4]),
                static_cast<unsigned long long>(row[5]),
                {to_uuid(row[6])}};
    }
    RegistrarCertification operator()(const RegistrarCertificationUuid& certification_uuid) const
    {
        const auto dbres = ctx_.get_conn().exec_params(
                "DELETE FROM registrar_certification "
                 "WHERE uuid = $1::UUID "
             "RETURNING id, "
                       "uuid, "
                       "valid_from, "
                       "valid_until, "
                       "classification, "
                       "eval_file_id, "
                       "eval_file_uuid",
                Database::QueryParams{certification_uuid.value});
        if (dbres.size() <= 0)
        {
            struct NotFound : RegistrarCertificationDoesNotExist
            {
                const char* what() const noexcept override { return "registrar certification not found by uuid"; }
            };
            throw NotFound{};
        }
        if (1 < dbres.size())
        {
            struct TooManyCertifications : std::exception
            {
                const char* what() const noexcept override { return "too many registrar certifications found by uuid"; }
            };
            throw TooManyCertifications{};
        }
        const auto row = dbres[0];
        return RegistrarCertification{
                static_cast<unsigned long long>(row[0]),
                {to_uuid(row[1])},
                boost::gregorian::from_string(static_cast<std::string>(row[2])),
                row[3].isnull() ? boost::gregorian::date{boost::date_time::special_values::pos_infin}
                                : boost::gregorian::from_string(static_cast<std::string>(row[3])),
                static_cast<int>(row[4]),
                static_cast<unsigned long long>(row[5]),
                {to_uuid(row[6])}};
    }
private:
    const OperationContext& ctx_;
};

} // namespace LibFred::Registrar::{anonymous}

unsigned long long DeleteRegistrarCertification::exec(const OperationContext& ctx) const
{
    return delete_registrar_certification(ctx, certification_id_).id;
}

} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar;

RegistrarCertification LibFred::Registrar::delete_registrar_certification(
        const OperationContext& ctx,
        const DeleteRegistrarCertification::RegistrarCertificationIdVariant& certification_id)
{
    try
    {
        return boost::apply_visitor(Exec{ctx}, certification_id);
    }
    catch (const std::exception& e)
    {
        FREDLOG_INFO(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to delete registrar certification due to an unknown exception");
        throw;
    }
}
