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

#include "libfred/registrar/certification/create_registrar_certification.hh"
#include "libfred/registrar/certification/exceptions.hh"

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <utility>

namespace LibFred {
namespace Registrar {

CreateRegistrarCertification::CreateRegistrarCertification(
        RegistrarIdVariant _registrar_id,
        boost::gregorian::date _valid_from,
        int _classification,
        FileIdVariant _eval_file_id)
    : registrar_id_{std::move(_registrar_id)},
      valid_from_{std::move(_valid_from)},
      valid_until_{boost::date_time::special_values::pos_infin},
      classification_{_classification},
      eval_file_id_{std::move(_eval_file_id)}
{ }

CreateRegistrarCertification::CreateRegistrarCertification(
        RegistrarIdVariant _registrar_id,
        boost::gregorian::date _valid_from,
        boost::gregorian::date _valid_until,
        int _classification,
        FileIdVariant _eval_file_id)
    : registrar_id_{std::move(_registrar_id)},
      valid_from_{std::move(_valid_from)},
      valid_until_{std::move(_valid_until)},
      classification_{_classification},
      eval_file_id_{std::move(_eval_file_id)}
{ }

CreateRegistrarCertification& CreateRegistrarCertification::set_valid_until(boost::gregorian::date _valid_until)
{
    valid_until_ = _valid_until;
    return *this;
}

namespace {

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

void check_validity(
        const OperationContext& ctx,
        const boost::gregorian::date& valid_from,
        const boost::gregorian::date& valid_until)
{
    if (valid_from.is_special())
    {
        throw InvalidDateFrom{};
    }
    if (!valid_until.is_special())
    {
        if (valid_until <= valid_from)
        {
            throw WrongIntervalOrder{};
        }
        const auto dbres = ctx.get_conn().exec_params(
                "SELECT $1::DATE < CURRENT_DATE",
                Database::QueryParams{valid_until});
        const bool valid_until_in_the_past = static_cast<bool>(dbres[0][0]);
        if (valid_until_in_the_past)
        {
            throw CertificationInPast{};
        }
    }
}

class LockedTableRegistrarCertification
{
public:
    explicit LockedTableRegistrarCertification(const OperationContext& ctx)
        : ctx_{ctx}
    {
        ctx_.get_conn().exec("LOCK TABLE registrar_certification IN ACCESS EXCLUSIVE MODE");
    }
    const OperationContext& ctx() const noexcept
    {
        return ctx_;
    }
private:
    const OperationContext& ctx_;
};

class MakeRegistrarIdSql : boost::static_visitor<std::string>
{
public:
    explicit MakeRegistrarIdSql(Database::QueryParams& params)
        : params_{params}
    { }
    std::string operator()(unsigned long long id) const
    {
        params_.emplace_back(id);
        return "$" + std::to_string(params_.size()) + "::BIGINT";
    }
    std::string operator()(const std::string& handle) const
    {
        params_.emplace_back(handle);
        return "(SELECT id FROM registrar WHERE handle = UPPER($" + std::to_string(params_.size()) + "::TEXT))";
    }
private:
    Database::QueryParams& params_;
};

std::string make_registrar_id_sql(
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        Database::QueryParams& params)
{
    return boost::apply_visitor(MakeRegistrarIdSql{params}, registrar_id);
}

void truncate_validity(
        const LockedTableRegistrarCertification& locked_table,
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        const boost::gregorian::date& valid_from)
{
    Database::QueryParams params;
    const auto registrar_id_sql = make_registrar_id_sql(registrar_id, params);
    params.emplace_back(valid_from);
    const auto dbres = locked_table.ctx().get_conn().exec_params(
            // clang-format off
            "UPDATE registrar_certification "
               "SET valid_until = ($2::DATE - '1 DAY'::INTERVAL) "
             "WHERE registrar_id = " + registrar_id_sql + " AND "
                   "valid_from < $2::DATE AND "
                   "(valid_until IS NULL OR $2::DATE <= valid_until) "
         "RETURNING id",
            // clang-format on
            params);
    if (dbres.size() == 0)
    {
        FREDLOG_DEBUG("registrar certification no validity truncated");
        return;
    }
    if (dbres.size() == 1)
    {
        FREDLOG_INFO(boost::format{"registrar certification %1% validity truncated"} % static_cast<unsigned long long>(dbres[0][0]));
        return;
    }
    struct ValidityOverlaps : OverlappingRange
    {
        const char* what() const noexcept override { return "validity of certifications overlaps"; }
    };
    throw ValidityOverlaps{};
}

void check_validity_overlapping(
        const LockedTableRegistrarCertification& locked_table,
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        const boost::gregorian::date& valid_from)
{
    Database::QueryParams params;
    const auto registrar_id_sql = make_registrar_id_sql(registrar_id, params);
    params.emplace_back(valid_from);
    const auto dbres = locked_table.ctx().get_conn().exec_params(
            // clang-format off
            "SELECT EXISTS(SELECT 0 "
                            "FROM registrar_certification "
                           "WHERE registrar_id = registrar.id AND "
                                 "(valid_until IS NULL OR $2::DATE <= valid_until)) "
              "FROM registrar "
             "WHERE id = " + registrar_id_sql,
            // clang-format on
            params);
    if (dbres.size() <= 0)
    {
        throw RegistrarNotFound{};
    }
    if (dbres.size() != 1)
    {
        struct TooManyRegistrars : std::runtime_error
        {
            explicit TooManyRegistrars() : std::runtime_error{{}} { }
            const char* what() const noexcept override { return "too many registrars"; }
        };
        throw TooManyRegistrars{};
    }
    if (static_cast<bool>(dbres[0][0]))
    {
        struct ValidityWouldOverlap : OverlappingRange
        {
            const char* what() const noexcept override { return "validity of certifications would overlap"; }
        };
        throw ValidityWouldOverlap{};
    }
}

auto to_uuid(const Database::Value& value)
{
    return boost::uuids::string_generator()(static_cast<std::string>(value));
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
        return "files.id = $" + std::to_string(params_.size()) + "::BIGINT";
    }
    std::string operator()(const FileUuid& uuid) const
    {
        params_.emplace_back(uuid.value);
        return "files.uuid = $" + std::to_string(params_.size()) + "::UUID";
    }
private:
    Database::QueryParams& params_;
};

std::string make_file_id_sql(
        const CreateRegistrarCertification::FileIdVariant& file_id,
        Database::QueryParams& params)
{
    return boost::apply_visitor(MakeFileIdSql{params}, file_id);
}

decltype(auto) insert_one_record(
        const LockedTableRegistrarCertification& locked_table,
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        const boost::gregorian::date& valid_from,
        const boost::gregorian::date& valid_until,
        int classification,
        const CreateRegistrarCertification::FileIdVariant& eval_file_id)
{
    Database::QueryParams params;
    const auto registrar_id_sql = make_registrar_id_sql(registrar_id, params);
    params.emplace_back(valid_from);
    params.push_back(valid_until.is_special() ? Database::QPNull
                                              : valid_until);
    params.emplace_back(classification);
    const auto file_id_sql = make_file_id_sql(eval_file_id, params);
    const auto dbres = locked_table.ctx().get_conn().exec_params(
            // clang-format off
            "INSERT INTO registrar_certification "
                   "(registrar_id, "
                    "valid_from, "
                    "valid_until, "
                    "classification, "
                    "eval_file_uuid) "
            "SELECT " + registrar_id_sql + ", "
                   "$2::DATE, "
                   "$3::DATE, "
                   "$4::INTEGER, "
                   "files.uuid "
              "FROM files "
             "WHERE " + file_id_sql + " "
         "RETURNING id, "
                   "uuid, "
                   "valid_from, "
                   "valid_until, "
                   "classification, "
                   "eval_file_id, "
                   "eval_file_uuid",
            // clang-format on
            params);
    if (dbres.size() <= 0)
    {
        struct EvalFileDoesNotExist : FileDoesNotExist
        {
            const char* what() const noexcept override { return "eval file not found by uuid"; }
        };
        throw EvalFileDoesNotExist{};
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

}//namespace LibFred::Registrar::{anonymous}

unsigned long long CreateRegistrarCertification::exec(const OperationContext& _ctx) const
{
    return create_registrar_certification(
            _ctx,
            registrar_id_,
            valid_from_,
            valid_until_,
            classification_,
            eval_file_id_).id;
}

} // namespace LibFred::Registrar
} // namespace LibFred

using namespace LibFred::Registrar;

RegistrarCertification LibFred::Registrar::create_registrar_certification(
        const OperationContext& ctx,
        const CreateRegistrarCertification::RegistrarIdVariant& registrar_id,
        const boost::gregorian::date& valid_from,
        const boost::gregorian::date& valid_until,
        int classification,
        const CreateRegistrarCertification::FileIdVariant& eval_file_id)
{
    try
    {
        check_classification(classification);
        check_validity(ctx, valid_from, valid_until);
        LockedTableRegistrarCertification locked_table{ctx};
        truncate_validity(locked_table, registrar_id, valid_from);
        check_validity_overlapping(locked_table, registrar_id, valid_from);
        return insert_one_record(
                locked_table,
                registrar_id,
                valid_from,
                valid_until,
                classification,
                eval_file_id);
    }
    catch (const std::exception& e)
    {
        FREDLOG_INFO(e.what());
        throw;
    }
    catch (...)
    {
        FREDLOG_INFO("Failed to create registrar certification due to unknown exception");
        throw;
    }
}
