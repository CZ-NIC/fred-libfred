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

#include "libfred/registrar/certification/update_registrar_certification.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utility>

namespace LibFred {
namespace Registrar {

UpdateRegistrarCertification::UpdateRegistrarCertification(const RegistrarCertificationIdVariant& _certification_id)
    : certification_id_{_certification_id}
{ }

UpdateRegistrarCertification::UpdateRegistrarCertification(
        const RegistrarCertificationIdVariant& _certification_id,
        const boost::gregorian::date& valid_until)
    : certification_id_{_certification_id},
      valid_until_{valid_until}
{ }

UpdateRegistrarCertification::UpdateRegistrarCertification(
        const RegistrarCertificationIdVariant& _certification_id,
        int classification,
        const FileIdVariant& eval_file_id)
    : certification_id_{_certification_id},
      classification_{classification},
      eval_file_id_{eval_file_id}
{ }

UpdateRegistrarCertification& UpdateRegistrarCertification::set_valid_until(const boost::gregorian::date& valid_until)
{
    valid_until_ = valid_until;
    return *this;
}

UpdateRegistrarCertification& UpdateRegistrarCertification::set_classification(int classification)
{
    classification_ = classification;
    return *this;
}

UpdateRegistrarCertification& UpdateRegistrarCertification::set_eval_file_id(const FileIdVariant& eval_file_id)
{
    eval_file_id_ = eval_file_id;
    return *this;
}

namespace {

class MakeCertificationIdSql : boost::static_visitor<std::string>
{
public:
    explicit MakeCertificationIdSql(Database::QueryParams& params)
        : params_{params}
    { }
    std::string operator()(unsigned long long id) const
    {
        params_.emplace_back(id);
        return "id = $" + std::to_string(params_.size()) + "::BIGINT";
    }
    std::string operator()(const RegistrarCertificationUuid& uuid) const
    {
        params_.emplace_back(uuid.value);
        return "uuid = $" + std::to_string(params_.size()) + "::UUID";
    }
private:
    Database::QueryParams& params_;
};

std::string make_certification_id_sql(
        const UpdateRegistrarCertification::RegistrarCertificationIdVariant& certification_id,
        Database::QueryParams& params)
{
    return boost::apply_visitor(MakeCertificationIdSql{params}, certification_id);
}

void check_valid_until(
        const OperationContext& ctx,
        const UpdateRegistrarCertification::RegistrarCertificationIdVariant& certification_id,
        const boost::gregorian::date& valid_until)
{
    Database::QueryParams params;
    params.emplace_back(valid_until);
    const auto certification_id_sql = make_certification_id_sql(certification_id, params);
    const auto dbres = ctx.get_conn().exec_params(
            // clang-format off
            "WITH time_zone AS "
            "("
                "SELECT COALESCE((SELECT val FROM enum_parameters WHERE name = 'regular_day_procedure_zone'), "
                                "'UTC') AS val"
            ") "
            "SELECT $1::DATE < (NOW() AT TIME ZONE time_zone.val)::DATE, "
                    "$1::DATE < rc.valid_from "
                "FROM time_zone, "
                    "registrar_certification rc "
                "WHERE rc." + certification_id_sql + " "
        "FOR UPDATE OF rc",
            // clang-format on
            params);
    if (dbres.size() <= 0)
    {
        struct NotFound : RegistrarCertificationDoesNotExist
        {
            const char* what() const noexcept override { return "registrar certification not found"; }
        };
        throw NotFound{};
    }
    if (1 < dbres.size())
    {
        struct TooManyCertifications : std::exception
        {
            const char* what() const noexcept override { return "too many registrar certifications found"; }
        };
        throw TooManyCertifications{};
    }
    if (static_cast<bool>(dbres[0][0]))
    {
        struct TooOld : CertificationInPast
        {
            const char* what() const noexcept override { return "valid_until in the past"; }
        };
        throw TooOld{};
    }
    if (static_cast<bool>(dbres[0][1]))
    {
        struct ValidityMismatch : WrongIntervalOrder
        {
            const char* what() const noexcept override { return "valid_until older than valid_from"; }
        };
        throw ValidityMismatch{};
    }
}

void check_classification(int classification)
{
    static constexpr int min_classification_value = 0;
    static constexpr int max_classification_value = 5;
    const bool classification_is_out_of_range =
            (classification < min_classification_value) || (max_classification_value < classification);
    if (classification_is_out_of_range)
    {
        throw ScoreOutOfRange();
    }
}

class MakeFileIdSql : boost::static_visitor<std::string>
{
public:
    explicit MakeFileIdSql(Database::QueryParams& params)
        : params_{params}
    { }
    std::string operator()(unsigned long long id) const
    {
        params_.emplace_back(id);
        return "id = $" + std::to_string(params_.size()) + "::BIGINT";
    }
    std::string operator()(const FileUuid& uuid) const
    {
        params_.emplace_back(uuid.value);
        return "uuid = $" + std::to_string(params_.size()) + "::UUID";
    }
private:
    Database::QueryParams& params_;
};

void check_file(
        const OperationContext& ctx,
        const UpdateRegistrarCertification::FileIdVariant& file_id)
{
    Database::QueryParams params;
    const auto file_id_sql = boost::apply_visitor(MakeFileIdSql{params}, file_id);
    const auto dbres = ctx.get_conn().exec_params(
            // clang-format off
            "SELECT 0 FROM files WHERE " + file_id_sql,
            // clang-format on
            params);
    if (dbres.size() == 1)
    {
        return;
    }
    if (dbres.size() <= 0)
    {
        struct NotFound : FileDoesNotExist
        {
            const char* what() const noexcept override { return "file not found"; }
        };
        throw NotFound{};
    }
    struct TooManyFiles : std::exception
    {
        const char* what() const noexcept override { return "too many files found"; }
    };
    throw TooManyFiles{};
}

class MakeSetFileIdSql : boost::static_visitor<std::string>
{
public:
    explicit MakeSetFileIdSql(Database::QueryParams& params)
        : params_{params}
    { }
    std::string operator()(unsigned long long id) const
    {
        params_.emplace_back(id);
        return "eval_file_uuid = (SELECT uuid FROM files WHERE id = $" + std::to_string(params_.size()) + "::BIGINT)";
    }
    std::string operator()(const FileUuid& uuid) const
    {
        params_.emplace_back(uuid.value);
        return "eval_file_uuid = $" + std::to_string(params_.size()) + "::UUID";
    }
private:
    Database::QueryParams& params_;
};

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
}

} // namespace LibFred::Registrar::{anonymous}

unsigned long long UpdateRegistrarCertification::exec(const OperationContext& ctx) const
{
    return update_registrar_certification(
        ctx,
        certification_id_,
        valid_until_,
        classification_,
        eval_file_id_).id;
}

} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar;

RegistrarCertification LibFred::Registrar::update_registrar_certification(
        const OperationContext& ctx,
        const UpdateRegistrarCertification::RegistrarCertificationIdVariant& certification_id,
        const boost::optional<boost::gregorian::date>& valid_until,
        const boost::optional<int>& classification,
        const boost::optional<UpdateRegistrarCertification::FileIdVariant>& eval_file_id)
{
    try
    {
        Database::QueryParams params;
        std::string sql_set_part;
        if (valid_until != boost::none)
        {
            if (valid_until->is_special())
            {
                throw InvalidDateTo{};
            }
            check_valid_until(ctx, certification_id, *valid_until);
            params.emplace_back(*valid_until);
            sql_set_part = "valid_until = $1::DATE";
        }
        if (classification != boost::none)
        {
            check_classification(*classification);
            if (!sql_set_part.empty())
            {
                sql_set_part += ", ";
            }
            params.emplace_back(*classification);
            sql_set_part += "classification = $" + std::to_string(params.size()) + "::classification_type";
        }
        if (eval_file_id != boost::none)
        {
            check_file(ctx, *eval_file_id);
            if (!sql_set_part.empty())
            {
                sql_set_part += ", ";
            }
            sql_set_part += boost::apply_visitor(MakeSetFileIdSql{params}, *eval_file_id);
        }
        const auto certification_id_part = make_certification_id_sql(certification_id, params);
        const auto dbres = ctx.get_conn().exec_params(
                "UPDATE registrar_certification "
                   "SET " + sql_set_part + " "
                 "WHERE " + certification_id_part + " "
             "RETURNING id, "
                       "uuid, "
                       "valid_from, "
                       "valid_until, "
                       "classification, "
                       "eval_file_id, "
                       "eval_file_uuid",
                params);
        if (dbres.size() <= 0)
        {
            struct NotFound : RegistrarCertificationDoesNotExist
            {
                const char* what() const noexcept override { return "registrar certification not found"; }
            };
            throw NotFound{};
        }
        if (1 < dbres.size())
        {
            struct TooManyCertifications : std::exception
            {
                const char* what() const noexcept override { return "too many registrar certifications found"; }
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
    catch (const std::exception& e)
    {
        FREDLOG_INFO(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to update registrar certification due to an unknown exception");
        throw;
    }
}
