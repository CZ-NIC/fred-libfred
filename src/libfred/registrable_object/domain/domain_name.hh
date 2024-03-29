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
/**
 *  @file domain_name.hh
 *  domain name check
 */

#ifndef DOMAIN_NAME_HH_EE9936460E934E02B760C257E5DCAFEB
#define DOMAIN_NAME_HH_EE9936460E934E02B760C257E5DCAFEB

#include "libfred/opcontext.hh"
#include "util/factory.hh"
#include "util/factory_check.hh"
#include "util/optional_value.hh"

#include <string>
#include <vector>
#include <memory>

namespace LibFred {
namespace Domain {

/// returns true if _fqdn is RFC1123 section 2.1 compliant host name (however, final dot '.' is optional)
bool is_rfc1123_compliant_host_name(const std::string& fqdn);

class ExceptionInvalidFqdn : public std::exception {};
class ExceptionInvalidLabelCount : public std::exception {};

class DomainName
{
public:
    /**
     * @throw ExceptionInvalidFqdn in case fqdn_ is not valid by RFC 1035 mandatory rules
     */
    explicit DomainName(const std::string& _fqdn);
    /**
     * @throw ExceptionInvalidFqdn in case fqdn_ is not valid by RFC 1035 mandatory rules
     */
    explicit DomainName(const char* _fqdn);
    std::string get_string() const;
    /// Returns vector of labels - delimiting dots are not present in labels
    std::vector<std::string> get_labels()const
    {
        return labels_;
    }
    /*! \brief Returns subset of labels in this fqdn
     * @param[in] top_labels_to_skip how many top labels (from the "right") to ommit
     */
    DomainName get_subdomains(int _top_labels_to_skip)const;
private:
    std::vector<std::string> labels_;
    /**
     * @throw ExceptionInvalidFqdn in case fqdn_ is not valid by RFC 1035 mandatory rules
     */
    void init(const char* _fqdn);
};

class DomainNameChecker
{
public:
    virtual ~DomainNameChecker(){}
    virtual bool validate(const DomainName& domain_name) = 0;
};

class DomainNameCheckerNeedZoneName
{
public:
    virtual void set_zone_name(DomainName zone_name) = 0;
protected:
   virtual ~DomainNameCheckerNeedZoneName() {}
};

class DomainNameCheckerNeedOperationContext
{
public:
    virtual void set_ctx(const OperationContext& ctx) = 0;
protected:
   virtual ~DomainNameCheckerNeedOperationContext() {}
};

class ExceptionZoneNameNotSet : public std::exception {};
class ExceptionCtxNotSet : public std::exception {};

class DomainNameValidator
{
public:
    explicit DomainNameValidator(bool _is_system_registrar = false);

    ///set zone name if checker implementaion need one
    DomainNameValidator& set_zone_name(DomainName _zone_name);
    ///set operation context if checker implementaion need one
    DomainNameValidator& set_ctx(const OperationContext& _ctx);
    ///add checker name, checker names should be unique in instance of the validator
    DomainNameValidator& add(std::string checker_name);
    ///set checker names, forget previous set of names
    DomainNameValidator& set_checker_names(std::vector<std::string> checker_names);
    /*! \brief Returns true if domain name is valid otherwise it returns false.
     * @throw ZoneNameNotSet in case Zone name have not been set and checker which needs it was added.
     * @throw CtxNotSet in case OperationContext have not been set and checker which needs it was added.
     */
    bool exec(const DomainName& _fqdn, int _top_labels_to_skip = 0);
private:
    std::unique_ptr<DomainName> zone_name_;
    Optional<const OperationContext*> ctx_;
    const bool is_system_registrar_;
    std::vector<std::string> checker_name_vector_;//TODO: std::set might be better here
};

using DomainNameCheckerFactory = Util::Factory<DomainNameChecker>;

const DomainNameCheckerFactory& get_default_domain_name_checker_factory();

static const std::string DNCHECK_NO_CONSECUTIVE_HYPHENS = "dncheck_no_consecutive_hyphens";
static const std::string DNCHECK_RFC1035_PREFERRED_SYNTAX = "dncheck_rfc1035_preferred_syntax";
static const std::string DNCHECK_SINGLE_DIGIT_LABELS_ONLY = "dncheck_single_digit_labels_only";
static const std::string DNCHECK_NO_IDN_PUNYCODE = "dncheck_no_idn_punycode";

///trivial checker for testing
static const std::string DNCHECK_NOT_EMPTY_DOMAIN_NAME = "dncheck_not_empty_domain_name";

///insert checker name into database
void insert_domain_name_checker_name_into_database(
        const OperationContext& ctx,
        const std::string& checker_name,
        const std::string& checker_description);

///set domain name checkers for given zone
void set_domain_name_validation_config_into_database(
        const OperationContext& ctx,
        const std::string& zone_name,
        const std::vector<std::string>& checker_names);

///get domain name checkers for given zone
std::vector<std::string> get_domain_name_validation_config_for_zone(
        const OperationContext& ctx,
        const std::string& zone_name);

}//namespace LibFred::Domain
}//namespace LibFred

#endif//DOMAIN_NAME_HH_EE9936460E934E02B760C257E5DCAFEB
