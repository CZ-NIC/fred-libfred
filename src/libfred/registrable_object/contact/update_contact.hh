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
 *  @file
 *  contact update
 */

#ifndef UPDATE_CONTACT_HH_039849D7E0FD4D099F4F6CAFB1A32093
#define UPDATE_CONTACT_HH_039849D7E0FD4D099F4F6CAFB1A32093

#include "libfred/opexception.hh"
#include "libfred/opcontext.hh"
#include "util/optional_value.hh"
#include "util/db/nullable.hh"
#include "util/printable.hh"

#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/contact/contact_enum.hh"
#include "libfred/object/object.hh"
#include "libfred/registrable_object/contact/place_address.hh"

#include <string>
#include <vector>
#include <set>

namespace LibFred {

/**
 * Update of contact exception tag.
 */
class UpdateContactET;

/**
 * Exception type for @ref UpdateContact.
 */
template <>
struct ExceptionTraits<UpdateContactET>
{
    DECLARE_EXCEPTION_DATA(unknown_contact_handle, std::string);/**< exception members for unknown contact handle generated by macro @ref DECLARE_EXCEPTION_DATA*/
    DECLARE_EXCEPTION_DATA(unknown_ssntype, std::string);/**< exception members for unknown type of identification of the contact generated by macro @ref DECLARE_EXCEPTION_DATA*/
    DECLARE_EXCEPTION_DATA(unknown_registrar_handle, std::string);/**< exception members for unknown registrar generated by macro @ref DECLARE_EXCEPTION_DATA*/
    DECLARE_EXCEPTION_DATA(unknown_country, std::string);/**< exception members for unknown country generated by macro @ref DECLARE_EXCEPTION_DATA*/
    DECLARE_EXCEPTION_DATA(forbidden_company_name_setting, std::string);/**< exception members for forbidden set of company name generated by macro @ref DECLARE_EXCEPTION_DATA*/

    template <class DERIVED_EXCEPTION> struct ExceptionTemplate
        : ExceptionData_unknown_registrar_handle<DERIVED_EXCEPTION>
        , ExceptionData_unknown_ssntype<DERIVED_EXCEPTION>
        , ExceptionData_unknown_country<DERIVED_EXCEPTION>
        , ExceptionData_forbidden_company_name_setting<DERIVED_EXCEPTION>
    {};

    /**
     * Exception for accumulation of non-fatal good path errors, fatal good path errors shall be reported immediately
     */
    struct Exception
        : virtual LibFred::OperationException
        , ExceptionData_unknown_contact_handle<Exception>
        , ExceptionTemplate<Exception>
    {};
protected:
    /**
     * Empty destructor meant to be called by derived class.
     */
    ~ExceptionTraits(){}
};


/**
 * Container of additional contact addresses to update.
 */
class ContactAddressToUpdate
{
public:
    /**
     * Default constructor, nothing to update, nothing to remove.
     */
    ContactAddressToUpdate() { }
    /**
     * Copy constructor.
     * @param _src refers to instance which is copied
     */
    ContactAddressToUpdate(const ContactAddressToUpdate &_src);
    /**
     * Destructor, nothing to do.
     */
    ~ContactAddressToUpdate() { }
    /**
     * Sets address given purpose to be updated.
     * @tparam type purpose of address
     * @param _address new content of contact address given purpose
     * @return self instance reference to allow method chaining
     */
    template <ContactAddressType::Value type>
    ContactAddressToUpdate& update(const ContactAddress &_address);
    /**
     * Contact address given purpose to be removed.
     * @tparam type purpose of address
     * @return self instance reference to allow method chaining
     */
    template <ContactAddressType::Value type>
    ContactAddressToUpdate& remove();
    /**
     * Container of addresses given purpose.
     */
    typedef ContactAddressList ToUpdate;
    /**
     * Container of purposes.
     */
    typedef std::set< ContactAddressType> ToRemove;
    /**
     * Get addresses to update.
     * @return addresses to update
     */
    const ToUpdate& to_update()const { return to_update_; }
    /**
     * Get addresses to remove.
     * @return addresses to remove
     */
    const ToRemove& to_remove()const { return to_remove_; }
    /**
     * Dumps content of the instance into the string
     * @return string with description of the instance content
     */
    std::string to_string()const;
private:
    ToUpdate to_update_;
    ToRemove to_remove_;
};

/**
 * Update of contact, implementation template.
 * Created instance is modifiable by chainable methods i.e. methods returning instance reference.
 * Data set into instance by constructor and methods serve as input data of the update.
 * Update is executed by @ref exec  method on contact instance identified by @ref InfoContactOutput parameter and using database connection supplied in @ref OperationContext parameter.
 * In case of wrong input data or other predictable and superable failure, i.e. good path error, an instance of @ref UpdateContact::ExceptionType is set or set and thrown depending on the fatality of the error.
 * In case of other insuperable failures and inconsistencies, i.e. bad path error, an instance of @ref InternalError or other exception is thrown.
 */
template <class DERIVED>
class UpdateContact
    : public ExceptionTraits<UpdateContactET>
{
public:
    /**
     * Update contact constructor with mandatory parameter.
     * @param registrar sets registrar handle into @ref registrar_ attribute
     */
    UpdateContact(const std::string& registrar)
        : registrar_(registrar)
    {}

    /**
     * Update contact constructor with all parameters.
     * @param registrar sets registrar handle into @ref registrar_ attribute
     * @param authinfo sets transfer password into @ref authinfo_ attribute
     * @param name sets name of contact person into @ref name_ attribute
     * @param organization sets full trade name of organization into @ref organization_ attribute
     * @param place sets place address of contact into @ref place_ attribute
     * @param telephone sets telephone number into @ref telephone_ attribute
     * @param fax sets fax number into @ref fax_ attribute
     * @param email sets e-mail address into @ref email_ attribute
     * @param notifyemail sets e-mail address for notifications into @ref notifyemail_ attribute
     * @param vat sets taxpayer identification number into @ref vat_ attribute
     * @param personal_id sets type and identification value into @ref personal_id_ attribute
     * @param addresses sets contact addresses into @ref addresses_ attribute
     * @param disclosename sets whether to reveal contact name into @ref disclosename_ attribute
     * @param discloseorganization sets whether to reveal organization name into @ref discloseorganization_ attribute
     * @param discloseaddress sets whether to reveal contact address into @ref discloseaddress_ attribute
     * @param disclosetelephone sets whether to reveal telephone number into @ref disclosetelephone_ attribute
     * @param disclosefax sets whether to reveal fax number into @ref disclosefax_ attribute
     * @param discloseemail sets whether to reveal e-mail address into @ref discloseemail_ attribute
     * @param disclosevat sets whether to reveal taxpayer identification number into @ref disclosevat_ attribute
     * @param discloseident sets whether to reveal unambiguous identification number into @ref discloseident_ attribute
     * @param disclosenotifyemail sets whether to reveal e-mail address for notifications into @ref disclosenotifyemail_ attribute
     * @param domain_expiration_warning_letter_enabled sets user preference whether to send domain expiration letters for domains linked to this contact, if TRUE then send domain expiration letters, if FALSE don't send domain expiration letters, if is NULL no user preference set
     * @param logd_request_id sets logger request id into @ref logd_request_id_ attribute
     */
    UpdateContact(const std::string& registrar
        , const Optional<std::string>& authinfo
        , const Optional<Nullable<std::string>>& name
        , const Optional<Nullable<std::string>>& organization
        , const Optional<Nullable<LibFred::Contact::PlaceAddress>>& place
        , const Optional<Nullable<std::string>>& telephone
        , const Optional<Nullable<std::string>>& fax
        , const Optional<Nullable<std::string>>& email
        , const Optional<Nullable<std::string>>& notifyemail
        , const Optional<Nullable<std::string>>& vat
        , const Optional<Nullable<PersonalIdUnion>>& personal_id
        , const ContactAddressToUpdate &addresses
        , const Optional<bool>& disclosename
        , const Optional<bool>& discloseorganization
        , const Optional<bool>& discloseaddress
        , const Optional<bool>& disclosetelephone
        , const Optional<bool>& disclosefax
        , const Optional<bool>& discloseemail
        , const Optional<bool>& disclosevat
        , const Optional<bool>& discloseident
        , const Optional<bool>& disclosenotifyemail
        , const Optional<Nullable<bool>>& domain_expiration_warning_letter_enabled
        , const Optional<unsigned long long>& logd_request_id);

    /**
     * Sets contact transfer password.
     * @param authinfo sets transfer password into @ref authinfo_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_authinfo(const std::string& authinfo)
    {
        authinfo_ = authinfo;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact name.
     * @param name sets name of contact person into @ref name_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_name(const Nullable<std::string>& name)
    {
        name_ = name;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact organization name.
     * @param organization sets full trade name of organization into @ref organization_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_organization(const Nullable<std::string>& organization)
    {
        organization_ = organization;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets place address of contact.
     * @param place sets place address of contact into @ref place_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_place(const Nullable<LibFred::Contact::PlaceAddress>& place)
    {
        place_ = place;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact telephone number.
     * @param telephone sets telephone number into @ref telephone_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_telephone(const Nullable<std::string>& telephone)
    {
        telephone_ = telephone;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact fax number.
     * @param fax sets fax number into @ref fax_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_fax(const Nullable<std::string>& fax)
    {
        fax_ = fax;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact e-mail address.
     * @param email sets e-mail address into @ref email_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_email(const Nullable<std::string>& email)
    {
        email_ = email;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact e-mail address for notifications.
     * @param notifyemail sets e-mail address for notifications into @ref notifyemail_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_notifyemail(const Nullable<std::string>& notifyemail)
    {
        notifyemail_ = notifyemail;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact taxpayer identification number.
     * @param vat sets taxpayer identification number into @ref vat_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_vat(const Nullable<std::string>& vat)
    {
        vat_ = vat;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets contact unique identification type and value.
     * @param personal_id sets type and value of identification into @ref personal_id_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_personal_id(const Nullable<PersonalIdUnion>& personal_id)
    {
        personal_id_ = personal_id;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets address given purpose to be updated.
     * @tparam type purpose of address
     * @param _address new content of contact address given purpose
     * @return self instance reference to allow method chaining
     */
    template <ContactAddressType::Value type>
    DERIVED& set_address(const ContactAddress& _address)
    {
        addresses_.update<type>(_address);
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Contact address given purpose to be removed.
     * @tparam type purpose of address
     * @return self instance reference to allow method chaining
     */
    template <ContactAddressType::Value type>
    DERIVED& reset_address()
    {
        addresses_.remove<type>();
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal contact name.
     * @param disclosename sets whether to reveal contact name into @ref disclosename_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_disclosename(bool disclosename)
    {
        disclosename_ = disclosename;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal organization name.
     * @param discloseorganization sets whether to reveal organization name into @ref discloseorganization_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_discloseorganization(bool discloseorganization)
    {
        discloseorganization_ = discloseorganization;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal address.
     * @param discloseaddress sets whether to reveal contact address into @ref discloseaddress_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_discloseaddress(bool discloseaddress)
    {
        discloseaddress_ = discloseaddress;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal telephone number.
     * @param disclosetelephone sets whether to reveal telephone number into @ref disclosetelephone_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_disclosetelephone(bool disclosetelephone)
    {
        disclosetelephone_ = disclosetelephone;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal fax number.
     * @param disclosefax sets whether to reveal fax number into @ref disclosefax_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_disclosefax(bool disclosefax)
    {
        disclosefax_ = disclosefax;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal e-mail address.
     * @param discloseemail sets whether to reveal e-mail address into @ref discloseemail_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_discloseemail(bool discloseemail)
    {
        discloseemail_ = discloseemail;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal taxpayer identification number.
     * @param disclosevat sets whether to reveal taxpayer identification number into @ref disclosevat_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_disclosevat(bool disclosevat)
    {
        disclosevat_ = disclosevat;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal unambiguous identification number.
     * @param discloseident sets whether to reveal unambiguous identification number into @ref discloseident_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_discloseident(bool discloseident)
    {
        discloseident_ = discloseident;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets whether to reveal e-mail address for notifications.
     * @param disclosenotifyemail sets whether to reveal e-mail address for notifications into @ref disclosenotifyemail_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_disclosenotifyemail(bool disclosenotifyemail)
    {
        disclosenotifyemail_ = disclosenotifyemail;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Sets user preference whether to send domain expiration letters for domains linked to this contact.
     * @param domain_expiration_warning_letter_enabled sets user preference whether to send domain expiration letters for domains linked to this contact into @ref domain_expiration_warning_letter_enabled_ attribute.
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_domain_expiration_warning_letter_enabled(const Nullable<bool>& domain_expiration_warning_letter_enabled)
    {
        domain_expiration_warning_letter_enabled_ = domain_expiration_warning_letter_enabled;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Unsets user preference whether to send domain expiration letters for domains linked to this contact.
     * Sets NULL into @ref domain_expiration_warning_letter_enabled_ attribute.
     * @return operation instance reference to allow method chaining
     */
    DERIVED& unset_domain_expiration_warning_letter_enabled()
    {
        return this->set_domain_expiration_warning_letter_enabled(Nullable<bool>());
    }

    /**
     * Sets logger request id
     * @param logd_request_id sets logger request id into @ref logd_request_id_ attribute
     * @return operation instance reference to allow method chaining
     */
    DERIVED& set_logd_request_id(unsigned long long logd_request_id)
    {
        logd_request_id_ = logd_request_id;
        return static_cast<DERIVED&>(*this);
    }

    /**
     * Executes update
     * @param ctx contains reference to database and logging interface
     * @param contact designated for the update
     * @return new history_id
     */
    unsigned long long exec(OperationContext& ctx, const InfoContactOutput& contact);

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string()const;
protected:
    /**
     * Empty destructor meant to be called by derived class.
     */
    ~UpdateContact() {}
private:
    const std::string registrar_;/**< handle of registrar performing the update */
    Optional<std::string> sponsoring_registrar_;/**< handle of registrar administering the object */
    Optional<std::string> authinfo_;/**< transfer password */
    Optional<Nullable<std::string>> name_ ;/**< name of contact person */
    Optional<Nullable<std::string>> organization_;/**< full trade name of organization */
    Optional<Nullable<LibFred::Contact::PlaceAddress>> place_;/**< place address of contact */
    Optional<Nullable<std::string>> telephone_;/**<  telephone number */
    Optional<Nullable<std::string>> fax_;/**< fax number */
    Optional<Nullable<std::string>> email_;/**< e-mail address */
    Optional<Nullable<std::string>> notifyemail_;/**< to this e-mail address will be send message in case of any change in domain or nsset affecting contact */
    Optional<Nullable<std::string>> vat_;/**< taxpayer identification number */
    Optional<Nullable<PersonalIdUnion>> personal_id_;/**< unambiguous identification number e.g. social security number, identity card number, date of birth */
    ContactAddressToUpdate addresses_;/**< contact addresses to update or remove */
    Optional<bool> disclosename_;/**< whether to reveal contact name */
    Optional<bool> discloseorganization_;/**< whether to reveal organization */
    Optional<bool> discloseaddress_;/**< whether to reveal address */
    Optional<bool> disclosetelephone_;/**< whether to reveal phone number */
    Optional<bool> disclosefax_;/**< whether to reveal fax number */
    Optional<bool> discloseemail_;/**< whether to reveal email address */
    Optional<bool> disclosevat_;/**< whether to reveal taxpayer identification number */
    Optional<bool> discloseident_;/**< whether to reveal unambiguous identification number */
    Optional<bool> disclosenotifyemail_;/**< whether to reveal notify email */
    Optional<Nullable<bool>> domain_expiration_warning_letter_enabled_;/**< user preference whether to send domain expiration letters for domains linked to this contact, if TRUE then send domain expiration letters, if FALSE don't send domain expiration letters, if is NULL no user preference set */
    Optional<unsigned long long> logd_request_id_; /**< id of the new entry in log_entry database table, id is used in other calls to logging within current request */
};

/**
 * @class UpdateContactById
 * Update of contact identified by database id.
 * Forward declaration of derived composite class.
 */
class UpdateContactById;

/**
 * Exception type for @ref UpdateContactById.
 */
template <>
struct ExceptionTraits<UpdateContactById>
{
    DECLARE_EXCEPTION_DATA(unknown_contact_id, unsigned long long);/**< exception members for unknown contact database id generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
        : virtual LibFred::OperationException
        , ExceptionData_unknown_contact_id<Exception>
        , ExceptionTraits<UpdateContactET>::ExceptionTemplate<Exception>
    {};
protected:
    /**
     * Empty destructor meant to be called by derived class.
     */
    ~ExceptionTraits(){}
};


class UpdateContactById
    : public Util::Printable<UpdateContactById>,
      public ExceptionTraits<UpdateContactById>,
      public UpdateContact<UpdateContactById>
{
public:
    typedef LibFred::ExceptionTraits<UpdateContactById>::Exception ExceptionType;/**< Exception type inherited via @ref ExceptionTraits */

    /**
     * Update contact by id constructor with mandatory parameters.
     * @param id sets registrar handle into id_ and InfoContactById members
     * @param registrar sets registrar handle into UpdateContact base
     */
    UpdateContactById(unsigned long long id, const std::string& registrar);

    /**
     * Update contact by id constructor with all parameters.
     * @param id sets database id into id_ and InfoContactById members
     * @param registrar sets registrar handle into UpdateContact base
     * @param authinfo sets transfer password into UpdateContact base
     * @param name sets name of contact person into UpdateContact base
     * @param organization sets full trade name of organization into UpdateContact base
     * @param place sets place address of contact into UpdateContact base
     * @param telephone sets telephone number into UpdateContact base
     * @param fax sets fax number into UpdateContact base
     * @param email sets e-mail address into UpdateContact base
     * @param notifyemail sets e-mail address for notifications into UpdateContact base
     * @param vat sets taxpayer identification number into UpdateContact base
     * @param personal_id sets type and value of identification into UpdateContact base
     * @param addresses sets contact addresses into UpdateContact base
     * @param disclosename sets whether to reveal contact name into UpdateContact base
     * @param discloseorganization sets whether to reveal organization name into UpdateContact base
     * @param discloseaddress sets whether to reveal contact address into UpdateContact base
     * @param disclosetelephone sets whether to reveal telephone number into UpdateContact base
     * @param disclosefax sets whether to reveal fax number into UpdateContact base
     * @param discloseemail sets whether to reveal e-mail address into UpdateContact base
     * @param disclosevat sets whether to reveal taxpayer identification number into UpdateContact base
     * @param discloseident sets whether to reveal unambiguous identification number into UpdateContact base
     * @param disclosenotifyemail sets whether to reveal e-mail address for notifications into UpdateContact base
     * @param domain_expiration_warning_letter_enabled sets user preference whether to send domain expiration letters for domains linked to this contact, if TRUE then send domain expiration letters, if FALSE don't send domain expiration letters, if is NULL no user preference set
     * @param logd_request_id sets logger request id into UpdateContact base
     */
    UpdateContactById(unsigned long long id
            , const std::string& registrar
            , const Optional<std::string>& authinfo
            , const Optional<Nullable<std::string>>& name
            , const Optional<Nullable<std::string>>& organization
            , const Optional<Nullable<LibFred::Contact::PlaceAddress>>& place
            , const Optional<Nullable<std::string>>& telephone
            , const Optional<Nullable<std::string>>& fax
            , const Optional<Nullable<std::string>>& email
            , const Optional<Nullable<std::string>>& notifyemail
            , const Optional<Nullable<std::string>>& vat
            , const Optional<Nullable<PersonalIdUnion>>& personal_id
            , const ContactAddressToUpdate &addresses
            , const Optional<bool>& disclosename
            , const Optional<bool>& discloseorganization
            , const Optional<bool>& discloseaddress
            , const Optional<bool>& disclosetelephone
            , const Optional<bool>& disclosefax
            , const Optional<bool>& discloseemail
            , const Optional<bool>& disclosevat
            , const Optional<bool>& discloseident
            , const Optional<bool>& disclosenotifyemail
            , const Optional<Nullable<bool>>& domain_expiration_warning_letter_enabled
            , const Optional<unsigned long long>& logd_request_id);

    /**
     * Executes update
     *
     * WARNING: May need to perform update on object_state_request as admin contact verification states might have been cancelled
     *
     * @param ctx contains reference to database and logging interface
     * @return new history_id
     */
    unsigned long long exec(LibFred::OperationContext& ctx);

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string()const;
private:
    unsigned long long id_;
    LibFred::InfoContactById select_contact_by_id_;
};

/**
 * @class UpdateContactByHandle
 * Update of contact identified by handle.
 * Forward declaration of derived composite class.
 */
class UpdateContactByHandle;

/**
 * Exception type for @ref UpdateContactByHandle.
 */
template <>
struct ExceptionTraits<UpdateContactByHandle>
{
    DECLARE_EXCEPTION_DATA(unknown_contact_handle, std::string);/**< exception members for unknown contact handle generated by macro @ref DECLARE_EXCEPTION_DATA*/
    struct Exception
    : virtual LibFred::OperationException
      , ExceptionData_unknown_contact_handle<Exception>
      , ExceptionTraits<UpdateContactET>::ExceptionTemplate<Exception>
    {};
protected:
    /**
     * Empty destructor meant to be called by derived class.
     */
    ~ExceptionTraits(){}
};


class UpdateContactByHandle
    : public Util::Printable<UpdateContactByHandle>,
      public ExceptionTraits<UpdateContactByHandle>,
      public UpdateContact<UpdateContactByHandle>
{
public:
    typedef LibFred::ExceptionTraits<UpdateContactByHandle>::Exception ExceptionType;/**< Exception type inherited via @ref ExceptionTraits */

    /**
     * Update contact by handle constructor with mandatory parameters.
     * @param handle sets contact handle into handle_ and InfoContactByHandle members
     * @param registrar sets registrar handle into UpdateContact base
     */
    UpdateContactByHandle(const std::string& handle, const std::string& registrar);

    /**
     * Update contact by handle constructor with mandatory parameters.
     * @param handle sets contact handle into handle_ and InfoContactByHandle members
     * @param registrar sets registrar handle into UpdateContact base
     * @param authinfo sets transfer password into UpdateContact base
     * @param name sets name of contact person into UpdateContact base
     * @param organization sets full trade name of organization into UpdateContact base
     * @param place sets place address of contact into UpdateContact base
     * @param telephone sets telephone number into UpdateContact base
     * @param fax sets fax number into UpdateContact base
     * @param email sets e-mail address into UpdateContact base
     * @param notifyemail sets e-mail address for notifications into UpdateContact base
     * @param vat sets taxpayer identification number into UpdateContact base
     * @param personal_id sets type and value of identification into UpdateContact base
     * @param addresses sets contact addresses into UpdateContact base
     * @param disclosename sets whether to reveal contact name into UpdateContact base
     * @param discloseorganization sets whether to reveal organization name into UpdateContact base
     * @param discloseaddress sets whether to reveal contact address into UpdateContact base
     * @param disclosetelephone sets whether to reveal telephone number into UpdateContact base
     * @param disclosefax sets whether to reveal fax number into UpdateContact base
     * @param discloseemail sets whether to reveal e-mail address into UpdateContact base
     * @param disclosevat sets whether to reveal taxpayer identification number into UpdateContact base
     * @param discloseident sets whether to reveal unambiguous identification number into UpdateContact base
     * @param disclosenotifyemail sets whether to reveal e-mail address for notifications into UpdateContact base
     * @param domain_expiration_warning_letter_enabled sets user preference whether to send domain expiration letters for domains linked to this contact, if TRUE then send domain expiration letters, if FALSE don't send domain expiration letters, if is NULL no user preference set
     * @param logd_request_id sets logger request id into UpdateContact base
     */
    UpdateContactByHandle(const std::string& handle
            , const std::string& registrar
            , const Optional<std::string>& authinfo
            , const Optional<Nullable<std::string>>& name
            , const Optional<Nullable<std::string>>& organization
            , const Optional<Nullable<LibFred::Contact::PlaceAddress>>& place
            , const Optional<Nullable<std::string>>& telephone
            , const Optional<Nullable<std::string>>& fax
            , const Optional<Nullable<std::string>>& email
            , const Optional<Nullable<std::string>>& notifyemail
            , const Optional<Nullable<std::string>>& vat
            , const Optional<Nullable<PersonalIdUnion>>& personal_id
            , const ContactAddressToUpdate& addresses
            , const Optional<bool>& disclosename
            , const Optional<bool>& discloseorganization
            , const Optional<bool>& discloseaddress
            , const Optional<bool>& disclosetelephone
            , const Optional<bool>& disclosefax
            , const Optional<bool>& discloseemail
            , const Optional<bool>& disclosevat
            , const Optional<bool>& discloseident
            , const Optional<bool>& disclosenotifyemail
            , const Optional<Nullable<bool>>& domain_expiration_warning_letter_enabled
            , const Optional<unsigned long long>& logd_request_id);

    /**
     * Executes update
     *
     * WARNING: May need to perform update on object_state_request as admin contact verification states might have been cancelled, but not here, keep that mess somewhere else
     *
     * @param ctx contains reference to database and logging interface
     * @return new history_id
     */
    unsigned long long exec(LibFred::OperationContext& ctx);

    /**
     * Dumps state of the instance into the string
     * @return string with description of the instance state
     */
    std::string to_string()const;
private:
    const std::string handle_;
    LibFred::InfoContactByHandle select_contact_by_handle_;
};

}//namespace LibFred

#endif//UPDATE_CONTACT_HH_039849D7E0FD4D099F4F6CAFB1A32093
