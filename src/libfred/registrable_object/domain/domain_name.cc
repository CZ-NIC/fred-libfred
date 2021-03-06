/*
 * Copyright (C) 2018-2019  CZ.NIC, z. s. p. o.
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
 *  @file domain_name.cc
 *  domain name check
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

FACTORY_MODULE_INIT_DEFI(domain_name_validator)

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
DomainNameValidator& DomainNameValidator::set_zone_name(const DomainName& _zone_name)
{
    zone_name_.reset(new DomainName(_zone_name));

    return *this;
}

DomainNameValidator& DomainNameValidator::set_ctx(OperationContext& _ctx)
{
    ctx_= &_ctx;

    return *this;
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


DomainName::DomainName(const std::string& _fqdn)
{
    init(_fqdn.c_str());
}

DomainName::DomainName(const char* const _fqdn)
{
    init(_fqdn);
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

DomainNameValidator& DomainNameValidator::add(const std::string& checker_name)
{
    FactoryHaveSupersetOfKeysChecker<LibFred::Domain::DomainNameCheckerFactory>
        ::KeyVector required_keys = boost::assign::list_of(checker_name);
    FactoryHaveSupersetOfKeysChecker<LibFred::Domain::DomainNameCheckerFactory>
        (required_keys).check();
    checker_name_vector_.push_back(checker_name);
    return *this;
}

DomainNameValidator& DomainNameValidator::set_checker_names(const std::vector<std::string>& checker_names)
{
    if (is_system_registrar_)
    {
        return *this;
    }

    for (std::vector<std::string>::const_iterator i = checker_names.begin(); i != checker_names.end() ; ++i) {}
    FactoryHaveSupersetOfKeysChecker<LibFred::Domain::DomainNameCheckerFactory>
        ::KeyVector required_keys = checker_names;
    FactoryHaveSupersetOfKeysChecker<LibFred::Domain::DomainNameCheckerFactory>
        (required_keys).check();
    checker_name_vector_ = checker_names;
    return *this;
}

bool DomainNameValidator::exec(const DomainName& _fqdn, int top_labels_to_skip)
{
    DomainName labels_to_check = _fqdn.get_subdomains(top_labels_to_skip);

    if (is_system_registrar_)
    {
        return true; // validation ok
    }

    for (std::vector<std::string>::const_iterator ci = checker_name_vector_.begin(); ci != checker_name_vector_.end(); ++ci)
    {
        std::shared_ptr<DomainNameChecker> checker = DomainNameCheckerFactory::instance_ref().create_sh_ptr(*ci);

        DomainNameCheckerNeedZoneName* const need_zone_checker =
                dynamic_cast<DomainNameCheckerNeedZoneName*>(checker.get());
        if (need_zone_checker != nullptr)
        {
            if (zone_name_ == nullptr)
            {
                throw ExceptionZoneNameNotSet();
            }
            need_zone_checker->set_zone_name(*zone_name_);
        }

        DomainNameCheckerNeedOperationContext* const need_ctx_checker =
                dynamic_cast<DomainNameCheckerNeedOperationContext*>(checker.get());
        if (need_ctx_checker != nullptr)
        {
            if (!ctx_.isset())
            {
                throw ExceptionCtxNotSet();
            }
            need_ctx_checker->set_ctx(*ctx_.get_value());
        }
        if (!checker->validate(labels_to_check))
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
      public DomainNameCheckerNeedOperationContext,
      public Util::FactoryAutoRegister<DomainNameChecker, DomainNameCheckerNotEmptyDomainName>
{
public:
    DomainNameCheckerNotEmptyDomainName()
    : ctx_ptr_(0)
    {}

    bool validate(const DomainName& relative_domain_name)
    {
        return !zone_name_->get_string().empty() && !relative_domain_name.get_string().empty() && ctx_ptr_;
    }

    void set_zone_name(const DomainName& _zone_name)
    {
        zone_name_.reset(new DomainName(_zone_name) );
    }

    void set_ctx(const OperationContext& _ctx)
    {
        ctx_ptr_ = &_ctx;
    }

    static std::string registration_name()
    {
        return DNCHECK_NOT_EMPTY_DOMAIN_NAME;
    }
private:
    std::unique_ptr<DomainName> zone_name_;
    const OperationContext* ctx_ptr_;
};

///check domain name according to RFC1035 section 2.3.1. Preferred name syntax
class CheckRFC1035PreferredNameSyntax
    : public DomainNameChecker,
      public Util::FactoryAutoRegister<DomainNameChecker, CheckRFC1035PreferredNameSyntax>
{
public:
    CheckRFC1035PreferredNameSyntax()
    {}

    bool validate(const DomainName& relative_domain_name)
    {
        const boost::regex RFC1035_NAME_SYNTAX(
            "(([A-Za-z]|[A-Za-z][-A-Za-z0-9]{0,61}[A-Za-z0-9])[.])*"//optional non-highest-level labels
            "([A-Za-z]|[A-Za-z][-A-Za-z0-9]{0,61}[A-Za-z0-9])"//mandatory highest-level label
        );
        return boost::regex_match(relative_domain_name.get_string(), RFC1035_NAME_SYNTAX);
    }

    static std::string registration_name()
    {
        return DNCHECK_RFC1035_PREFERRED_SYNTAX;
    }
};

///prohibit consecutive hyphens '--'
class CheckNoConsecutiveHyphens
    : public DomainNameChecker,
      public Util::FactoryAutoRegister<DomainNameChecker, CheckNoConsecutiveHyphens>
{
public:
    CheckNoConsecutiveHyphens(){}

    bool validate(const DomainName& relative_domain_name)
    {
        const boost::regex CONSECUTIVE_HYPHENS_SYNTAX("[-][-]");
        return !boost::regex_search(relative_domain_name.get_string(), CONSECUTIVE_HYPHENS_SYNTAX);
    }

    static std::string registration_name()
    {
        return DNCHECK_NO_CONSECUTIVE_HYPHENS;
    }
};

///check domain name for single digit labels
class CheckSingleDigitLabelsOnly
    : public DomainNameChecker,
      public Util::FactoryAutoRegister<DomainNameChecker, CheckSingleDigitLabelsOnly>
{
public:
    CheckSingleDigitLabelsOnly(){}

    bool validate(const DomainName& relative_domain_name)
    {
        const boost::regex SINGLE_DIGIT_LABELS_SYNTAX(
            "([0-9][.])*"//optional non-highest-level single digit labels labels
            "[0-9]"//mandatory highest-level single digit label
        );
        return boost::regex_match(relative_domain_name.get_string(), SINGLE_DIGIT_LABELS_SYNTAX);
    }

    static std::string registration_name()
    {
        return DNCHECK_SINGLE_DIGIT_LABELS_ONLY;
    }
};

///prohibit idn punycode ('xn--{punycode}')
class CheckNoIdnPunycode
    : public DomainNameChecker,
      public Util::FactoryAutoRegister<DomainNameChecker, CheckNoIdnPunycode>
{
public:
    CheckNoIdnPunycode(){}

    bool validate(const DomainName& relative_domain_name)
    {
        const std::string IDN_PUNYCODE_PREFIX("xn--");
        const std::vector<std::string> labels = relative_domain_name.get_labels(); // get labels (without zone labels)
        for (std::vector<std::string>::const_iterator label = labels.begin(); label != labels.end(); ++label)
        {
            if (boost::starts_with(*label, IDN_PUNYCODE_PREFIX))
            {
                return false;
            }
        }
        return true;
    }

    static std::string registration_name()
    {
        return DNCHECK_NO_IDN_PUNYCODE;
    }
};

}//namespace LibFred::Domain::{anonymous}

void insert_domain_name_checker_name_into_database(
        OperationContext& ctx,
        const std::string& checker_name,
        const std::string& checker_description)
{
    ctx.get_conn().exec_params("INSERT INTO enum_domain_name_validation_checker(name,description) "
                               "VALUES($1::text,$2::text)",
                               Database::query_param_list(checker_name)(checker_description));
}

void set_domain_name_validation_config_into_database(
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

std::vector<std::string> get_domain_name_validation_config_for_zone(
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

}//namespace LibFred::Domain
}//namespace LibFred
