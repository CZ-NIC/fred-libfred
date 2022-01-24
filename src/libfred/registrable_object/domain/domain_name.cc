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

#include "libfred/registrable_object/domain/domain_name.hh"

#include "libfred/zone/zone.hh"

#include <boost/regex.hpp>
#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

namespace LibFred {
namespace Domain {

bool is_rfc1123_compliant_host_name(const std::string& _fqdn)
{
    constexpr int fqdn_min_length = 1;
    constexpr int fqdn_max_length = 253; // 253 + 1 for "optional final dot" + 1 for "empty root label size" = 255
    constexpr int label_min_length = 1;
    constexpr int label_max_length = 63;

    const std::string letter_digit_hyphen_dot = "ABCDEFGHIJKLMNOPQRSTUVWXYZzabcdefghijklmnopqrstuvwxyz0123456789-.";
    const std::string label_delimiter = ".";

    const std::string fqdn = LibFred::Zone::rem_trailing_dot(_fqdn);

    if ((fqdn.length() < fqdn_min_length) ||
        (fqdn.length() > fqdn_max_length))
    {
        return false;
    }

    if (fqdn.find_first_not_of(letter_digit_hyphen_dot) != std::string::npos)
    {
        return false;
    }

    std::vector<std::string> labels;
    boost::split(labels, fqdn, boost::is_any_of(label_delimiter));

    for (std::vector<std::string>::const_iterator label = labels.begin(); label != labels.end(); ++label)
    {
        if ((label->length() < label_min_length) ||
            (label->length() > label_max_length) ||
            (boost::starts_with(*label, "-")) ||
            (boost::ends_with(*label, "-")))
        {
            return false;
        }
    }
    return true;
}

DomainNameValidator::DomainNameValidator(const bool _is_system_registrar)
    : is_system_registrar_(_is_system_registrar)
{ }

//domain name validator
DomainNameValidator& DomainNameValidator::set_zone_name(DomainName _zone_name)
{
    zone_name_ = std::make_unique<DomainName>(std::move(_zone_name));
    return *this;
}

DomainNameValidator& DomainNameValidator::set_ctx(OperationContext& _ctx)
{
    ctx_ = &_ctx;
    return *this;
}

DomainName::DomainName(const std::string& _fqdn)
    : DomainName{_fqdn.c_str()}
{ }

DomainName::DomainName(const char* const _fqdn)
{
    init(_fqdn);
}

void DomainName::init(const char* const _fqdn)
{
    if (_fqdn == nullptr)
    {
        throw ExceptionInvalidFqdn();
    }
    std::string temp_fqdn(_fqdn);

    if (!is_rfc1123_compliant_host_name(temp_fqdn))
    {
        throw ExceptionInvalidFqdn();
    }

    temp_fqdn = LibFred::Zone::rem_trailing_dot(temp_fqdn);
    boost::split(labels_, temp_fqdn, boost::is_any_of("."));
}

std::string DomainName::get_string() const
{
    return boost::join(labels_, ".");
}

DomainName DomainName::get_subdomains(int _top_labels_to_skip)const
{
    if (_top_labels_to_skip < 0 || _top_labels_to_skip > static_cast<int>(labels_.size()))
    {
        throw ExceptionInvalidLabelCount();
    }

    std::vector<std::string> selected_labels;

    selected_labels.assign(labels_.begin(), labels_.end() - _top_labels_to_skip);

    return DomainName(boost::join(selected_labels, "."));
}

DomainNameValidator& DomainNameValidator::add(std::string checker_name)
{
    Util::FactoryHaveSupersetOfKeys::require(get_default_domain_name_checker_factory(), {checker_name});
    checker_name_vector_.push_back(std::move(checker_name));
    return *this;
}

DomainNameValidator& DomainNameValidator::set_checker_names(std::vector<std::string> checker_names)
{
    if (is_system_registrar_)
    {
        return *this;
    }

    Util::FactoryHaveSupersetOfKeys::require(get_default_domain_name_checker_factory(), checker_names);
    checker_name_vector_ = std::move(checker_names);
    return *this;
}

bool DomainNameValidator::exec(const DomainName& _fqdn, int top_labels_to_skip)
{
    const auto labels_to_check = _fqdn.get_subdomains(top_labels_to_skip);

    if (is_system_registrar_)
    {
        return true; // validation ok
    }

    for (const auto& checker_name : checker_name_vector_)
    {
        DomainNameChecker& checker = get_default_domain_name_checker_factory()[checker_name];

        auto* const need_zone_checker = dynamic_cast<DomainNameCheckerNeedZoneName*>(&checker);
        if (need_zone_checker != nullptr)
        {
            if (zone_name_ == nullptr)
            {
                throw ExceptionZoneNameNotSet{};
            }
            need_zone_checker->set_zone_name(*zone_name_);
        }

        auto* const need_ctx_checker = dynamic_cast<DomainNameCheckerNeedOperationContext*>(&checker);
        if (need_ctx_checker != nullptr)
        {
            if (!ctx_.isset())
            {
                throw ExceptionCtxNotSet{};
            }
            need_ctx_checker->set_ctx(*ctx_.get_value());
        }

        if (!checker.validate(labels_to_check))
        {
            return false; //validation failed
        }
    }
    return true;//validation ok
}

namespace {

//trivial checker for testing
class DomainNameCheckerNotEmptyDomainName
    : public DomainNameChecker,
      public DomainNameCheckerNeedZoneName,
      public DomainNameCheckerNeedOperationContext
{
public:
    DomainNameCheckerNotEmptyDomainName()
        : ctx_ptr_{nullptr}
    {}
private:
    bool validate(const DomainName& relative_domain_name) override
    {
        return !zone_name_->get_string().empty() && !relative_domain_name.get_string().empty() && ctx_ptr_;
    }

    void set_zone_name(DomainName _zone_name) override
    {
        zone_name_ = std::make_unique<DomainName>(std::move(_zone_name));
    }

    void set_ctx(const OperationContext& _ctx) override
    {
        ctx_ptr_ = &_ctx;
    }

    std::unique_ptr<DomainName> zone_name_;
    const OperationContext* ctx_ptr_;
};

///check domain name according to RFC1035 section 2.3.1. Preferred name syntax
class CheckRFC1035PreferredNameSyntax : public DomainNameChecker
{
public:
    bool validate(const DomainName& relative_domain_name)
    {
        const auto rfc1035_name_syntax = boost::regex{
            "(([A-Za-z]|[A-Za-z][-A-Za-z0-9]{0,61}[A-Za-z0-9])[.])*"//optional non-highest-level labels
            "([A-Za-z]|[A-Za-z][-A-Za-z0-9]{0,61}[A-Za-z0-9])"};//mandatory highest-level label
        return boost::regex_match(relative_domain_name.get_string(), rfc1035_name_syntax);
    }
};

///prohibit consecutive hyphens '--'
class CheckNoConsecutiveHyphens : public DomainNameChecker
{
private:
    bool validate(const DomainName& relative_domain_name) override
    {
        const auto consecutive_hyphens_syntax = boost::regex{"[-][-]"};
        return !boost::regex_search(relative_domain_name.get_string(), consecutive_hyphens_syntax);
    }
};

///check domain name for single digit labels
class CheckSingleDigitLabelsOnly : public DomainNameChecker
{
private:
    bool validate(const DomainName& relative_domain_name) override
    {
        const auto single_digit_labels_syntax = boost::regex{
            "([0-9][.])*"//optional non-highest-level single digit labels labels
            "[0-9]"};//mandatory highest-level single digit label
        return boost::regex_match(relative_domain_name.get_string(), single_digit_labels_syntax);
    }
};

///prohibit idn punycode ('xn--{punycode}')
class CheckNoIdnPunycode : public DomainNameChecker
{
private:
    bool validate(const DomainName& relative_domain_name) override
    {
        const auto idn_punycode_prefix = std::string{"xn--"};
        const std::vector<std::string> labels = relative_domain_name.get_labels(); // get labels (without zone labels)
        for (std::vector<std::string>::const_iterator label = labels.begin(); label != labels.end(); ++label)
        {
            if (boost::starts_with(*label, idn_punycode_prefix))
            {
                return false;
            }
        }
        return true;
    }
};

}//namespace LibFred::Domain::{anonymous}

}//namespace LibFred::Domain
}//namespace LibFred

using namespace LibFred::Domain;

const LibFred::Domain::DomainNameCheckerFactory& LibFred::Domain::get_default_domain_name_checker_factory()
{
    static const auto factory = []()
    {
        DomainNameCheckerFactory factory{};
        factory.add_producer({DNCHECK_NOT_EMPTY_DOMAIN_NAME, std::make_unique<DomainNameCheckerNotEmptyDomainName>()})
               .add_producer({DNCHECK_RFC1035_PREFERRED_SYNTAX, std::make_unique<CheckRFC1035PreferredNameSyntax>()})
               .add_producer({DNCHECK_NO_CONSECUTIVE_HYPHENS, std::make_unique<CheckNoConsecutiveHyphens>()})
               .add_producer({DNCHECK_SINGLE_DIGIT_LABELS_ONLY, std::make_unique<CheckSingleDigitLabelsOnly>()})
               .add_producer({DNCHECK_NO_IDN_PUNYCODE, std::make_unique<CheckNoIdnPunycode>()});
        return factory;
    }();
    return factory;
}

void LibFred::Domain::insert_domain_name_checker_name_into_database(
        OperationContext& ctx,
        const std::string& checker_name,
        const std::string& checker_description)
{
    ctx.get_conn().exec_params("INSERT INTO enum_domain_name_validation_checker(name,description) "
                               "VALUES($1::text,$2::text)",
                               Database::query_param_list(checker_name)(checker_description));
}

void LibFred::Domain::set_domain_name_validation_config_into_database(
        OperationContext& ctx,
        const std::string& zone_name,
        const std::vector<std::string>& checker_names)
{
    Zone::Data zone = Zone::get_zone(ctx, zone_name);
    ctx.get_conn().exec_params(
            "DELETE FROM zone_domain_name_validation_checker_map WHERE zone_id = $1::bigint",
            Database::query_param_list(zone.id));
    for (std::vector<std::string>::const_iterator i = checker_names.begin(); i != checker_names.end(); ++i)
    {
        ctx.get_conn().exec_params(
                "INSERT INTO zone_domain_name_validation_checker_map(zone_id,checker_id) "
                "SELECT $1::bigint,id FROM enum_domain_name_validation_checker WHERE name=$2::text",
                Database::query_param_list(zone.id)(*i));
    }//for checker_names
}

std::vector<std::string> LibFred::Domain::get_domain_name_validation_config_for_zone(
        OperationContext& ctx,
        const std::string& zone_name)
{
    std::vector<std::string> checker_names;

    const Database::Result checker_names_res = ctx.get_conn().exec_params(
        "SELECT ch.name "
        "FROM enum_domain_name_validation_checker ch "
        "JOIN zone_domain_name_validation_checker_map cfg ON cfg.checker_id=ch.id "
        "JOIN zone z ON z.id=cfg.zone_id "
        "WHERE z.fqdn=LOWER($1::text)", Database::query_param_list(zone_name));

    for (Database::Result::size_type i = 0 ; i < checker_names_res.size(); ++i)
    {
        checker_names.push_back(static_cast<std::string>(checker_names_res[i][0]));
    }
    return checker_names;
}
