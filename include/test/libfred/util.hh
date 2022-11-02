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

#ifndef UTIL_HH_EF3AC5061B72408083458556AF1CEE43
#define UTIL_HH_EF3AC5061B72408083458556AF1CEE43

#include "test/setup/fixtures.hh"
#include "test/setup/fixtures_utils.hh"

#include "libfred/object/check_authinfo.hh"
#include "libfred/object/store_authinfo.hh"
#include "libfred/registrar/create_registrar.hh"
#include "libfred/registrar/info_registrar.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/domain/create_domain.hh"
#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrable_object/keyset/create_keyset.hh"
#include "libfred/registrable_object/keyset/info_keyset.hh"
#include "libfred/registrable_object/nsset/create_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"

#include <utility>

inline bool check_std_exception(const std::exception &e)
{
    return e.what()[0] != '\0';
}

namespace Test {

struct autocommitting_context : virtual Test::instantiate_db_template {
    ::LibFred::OperationContextCreator ctx;

    virtual ~autocommitting_context() {
        ctx.commit_transaction();
    }
};

struct has_registrar : Test::autocommitting_context
{
    has_registrar()
        : registrar{
            [](const ::LibFred::OperationContext& ctx)
            {
                const std::string reg_handle = "REGISTRAR1";
                ::LibFred::CreateRegistrar{
                        reg_handle,
                        "Name " + reg_handle,
                        "Organization " + reg_handle,
                        {"Street " + reg_handle},
                        "City " + reg_handle,
                        "PostalCode " + reg_handle,
                        "Telephone " + reg_handle,
                        "Email " + reg_handle,
                        "Url " + reg_handle,
                        "Dic " + reg_handle}.exec(ctx);
                return ::LibFred::InfoRegistrarByHandle{reg_handle}.exec(ctx).info_registrar_data;
            }(ctx)}
    { }
    ::LibFred::InfoRegistrarData registrar;
};

struct HasAuthinfo
{
    HasAuthinfo(
            const ::LibFred::OperationContext& ctx,
            const ::LibFred::Object::ObjectId& object_id,
            const unsigned long long registrar_id,
            std::string password)
        : password{std::move(password)}
    {
        static constexpr auto ttl = std::chrono::seconds{3600};
        ::LibFred::Object::StoreAuthinfo{object_id, registrar_id, ttl}.exec(ctx, this->password);
    }
    std::string password;
};

struct has_contact : has_registrar
{
    has_contact()
        : contact{
                [&]()
                {
                    const std::string contact_handle = "CONTACT1";

                    std::map<::LibFred::ContactAddressType, ::LibFred::ContactAddress> address_list;
                    address_list[::LibFred::ContactAddressType::MAILING] = ::LibFred::ContactAddress(
                        Optional<std::string>(),
                        ::LibFred::Contact::PlaceAddress(
                                "ulice 1", "ulice 2", "ulice 3",
                                "mesto", "kraj", "12345", "CZ"
                        )
                    );

                    Test::generate_test_data(::LibFred::CreateContact(contact_handle, registrar.handle))
                        .set_addresses(address_list)
                        .exec(ctx);

                    return ::LibFred::InfoContactByHandle(contact_handle).exec(ctx).info_contact_data;
                }()},
          contact_authinfo{ctx, ::LibFred::Object::ObjectId{contact.id}, registrar.id, "contact1 password"}
    { }
    ::LibFred::InfoContactData contact;
    HasAuthinfo contact_authinfo;
};

struct has_contact_and_a_different_registrar : has_contact
{
    has_contact_and_a_different_registrar()
        : has_contact{},
          the_different_registrar{
                [&]()
                {
                    const std::string diff_reg_handle = "REGISTRAR2";
                    ::LibFred::CreateRegistrar{
                            diff_reg_handle,
                            "Name " + diff_reg_handle,
                            "Organization " + diff_reg_handle,
                            {"Street " + diff_reg_handle},
                            "City " + diff_reg_handle,
                            "PostalCode " + diff_reg_handle,
                            "Telephone " + diff_reg_handle,
                            "Email " + diff_reg_handle,
                            "Url " + diff_reg_handle,
                            "Dic " + diff_reg_handle}.exec(ctx);
                    return ::LibFred::InfoRegistrarByHandle(diff_reg_handle).exec(ctx).info_registrar_data;
                }()}
    { }
    ::LibFred::InfoRegistrarData the_different_registrar;
};

struct has_domain : has_contact
{
    has_domain()
        : admin_contact1{
                [&]()
                {
                    const std::string admin_contact1_handle = "ADM-CONTACT1";
                    ::LibFred::CreateContact(admin_contact1_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(admin_contact1_handle).exec(ctx).info_contact_data;
                }()},
          admin_contact2{
                [&]()
                {
                    const std::string admin_contact2_handle = "ADM-CONTACT2";
                    ::LibFred::CreateContact(admin_contact2_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(admin_contact2_handle).exec(ctx).info_contact_data;
                }()},
          domain{
                [&]()
                {
                    const std::string fqdn = "domena.cz";
                    const auto admin_contacts = std::vector<std::string>{admin_contact1.handle, admin_contact2.handle};
                    Test::generate_test_data(::LibFred::CreateDomain(fqdn, registrar.handle, contact.handle))
                            .set_admin_contacts(admin_contacts)
                            .exec(ctx);
                    return ::LibFred::InfoDomainByFqdn(fqdn).exec(ctx).info_domain_data;
                }()},
          domain_authinfo{ctx, ::LibFred::Object::ObjectId{domain.id}, registrar.id, "domain password"},
          admin_contact1_authinfo{ctx, ::LibFred::Object::ObjectId{admin_contact1.id}, registrar.id, "domain admin contact1 password"},
          admin_contact2_authinfo{ctx, ::LibFred::Object::ObjectId{admin_contact2.id}, registrar.id, "domain admin contact2 password"}
    {}
    ::LibFred::InfoContactData admin_contact1;
    ::LibFred::InfoContactData admin_contact2;
    ::LibFred::InfoDomainData domain;
    HasAuthinfo domain_authinfo;
    HasAuthinfo admin_contact1_authinfo;
    HasAuthinfo admin_contact2_authinfo;
};

struct has_keyset : has_contact
{
    has_keyset()
        : tech_contact1{
                [&]()
                {
                    const std::string tech_contact1_handle = "TECH-CONTACT1";
                    ::LibFred::CreateContact(tech_contact1_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(tech_contact1_handle).exec(ctx).info_contact_data;
                }()},
          tech_contact2{
                [&]()
                {
                    const std::string tech_contact2_handle = "TECH-CONTACT2";
                    ::LibFred::CreateContact(tech_contact2_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(tech_contact2_handle).exec(ctx).info_contact_data;
                }()},
          keyset{
                [&]()
                {
                    const std::string handle = "KEYSET375";
                    const auto tech_contacts = std::vector<std::string>{tech_contact1.handle, tech_contact2.handle};
                    const auto dns_keys = std::vector<::LibFred::DnsKey>{
                        ::LibFred::DnsKey(257, 3, 5, "carymarypodkocary456"),
                        ::LibFred::DnsKey(255, 3, 5, "abrakadabra852")};
                    Test::generate_test_data(::LibFred::CreateKeyset(handle, registrar.handle))
                            .set_dns_keys(dns_keys)
                            .set_tech_contacts(tech_contacts)
                            .exec(ctx);
                    return ::LibFred::InfoKeysetByHandle(handle).exec(ctx).info_keyset_data;
                }()},
          keyset_authinfo{ctx, ::LibFred::Object::ObjectId{keyset.id}, registrar.id, "keyset password"},
          tech_contact1_authinfo{ctx, ::LibFred::Object::ObjectId{tech_contact1.id}, registrar.id, "keyset tech contact1 password"},
          tech_contact2_authinfo{ctx, ::LibFred::Object::ObjectId{tech_contact2.id}, registrar.id, "keyset tech contact2 password"}
    { }
    ::LibFred::InfoContactData tech_contact1;
    ::LibFred::InfoContactData tech_contact2;
    ::LibFred::InfoKeysetData keyset;
    HasAuthinfo keyset_authinfo;
    HasAuthinfo tech_contact1_authinfo;
    HasAuthinfo tech_contact2_authinfo;
};

struct has_nsset : has_contact
{
    has_nsset()
        : tech_contact1{
                [&]()
                {
                    const std::string tech_contact1_handle = "TECH-CONTACT1";
                    ::LibFred::CreateContact(tech_contact1_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(tech_contact1_handle).exec(ctx).info_contact_data;
                }()},
          tech_contact2{
                [&]()
                {
                    const std::string tech_contact2_handle = "TECH-CONTACT2";
                    ::LibFred::CreateContact(tech_contact2_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(tech_contact2_handle).exec(ctx).info_contact_data;
                }()},
          nsset{
                [&]()
                {
                    const std::string handle = "NSSET9875123";
                    const auto tech_contacts = std::vector<std::string>{tech_contact1.handle, tech_contact2.handle};
                    Test::generate_test_data(::LibFred::CreateNsset(handle, registrar.handle))
                            .set_tech_contacts(tech_contacts)
                            .set_tech_check_level(3)
                            .exec(ctx);
                    return ::LibFred::InfoNssetByHandle(handle).exec(ctx).info_nsset_data;
                }()},
          nsset_authinfo{ctx, ::LibFred::Object::ObjectId{nsset.id}, registrar.id, "nsset password"},
          tech_contact1_authinfo{ctx, ::LibFred::Object::ObjectId{tech_contact1.id}, registrar.id, "nsset tech contact1 password"},
          tech_contact2_authinfo{ctx, ::LibFred::Object::ObjectId{tech_contact2.id}, registrar.id, "nsset tech contact2 password"}
    { }
    ::LibFred::InfoContactData tech_contact1;
    ::LibFred::InfoContactData tech_contact2;
    ::LibFred::InfoNssetData nsset;
    HasAuthinfo nsset_authinfo;
    HasAuthinfo tech_contact1_authinfo;
    HasAuthinfo tech_contact2_authinfo;
};

struct has_enum_domain : has_contact
{
    has_enum_domain()
        : admin_contact1{
                [&]()
                {
                    const std::string admin_contact1_handle = "ADM-CONTACT1";
                    ::LibFred::CreateContact(admin_contact1_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(admin_contact1_handle).exec(ctx).info_contact_data;
                }()},
          admin_contact2{
                [&]()
                {
                    const std::string admin_contact2_handle = "ADM-CONTACT2";
                    ::LibFred::CreateContact(admin_contact2_handle, registrar.handle).exec(ctx);
                    return ::LibFred::InfoContactByHandle(admin_contact2_handle).exec(ctx).info_contact_data;
                }()},
          domain{
                [&]()
                {
                    const std::string fqdn = "1.1.1.1.1.1.1.1.1.0.2.4.e164.arpa";
                    const std::vector<std::string> admin_contacts = boost::assign::list_of(admin_contact1.handle)(admin_contact2.handle);
                    Test::generate_test_data( ::LibFred::CreateDomain(fqdn, registrar.handle, contact.handle) )
                        .set_admin_contacts(admin_contacts)
                        .set_enum_publish_flag(true)
                        .set_enum_validation_expiration(
                            boost::gregorian::from_string(
                                static_cast<std::string>( ctx.get_conn().exec("SELECT (now() + '1 year'::interval)::date")[0][0] )
                            )
                        )
                        .exec(ctx);
                    return ::LibFred::InfoDomainByFqdn(fqdn).exec(ctx).info_domain_data;
                }()},
          domain_authinfo{ctx, ::LibFred::Object::ObjectId{domain.id}, registrar.id, "domain password"},
          admin_contact1_authinfo{ctx, ::LibFred::Object::ObjectId{admin_contact1.id}, registrar.id, "domain admin contact1 password"},
          admin_contact2_authinfo{ctx, ::LibFred::Object::ObjectId{admin_contact2.id}, registrar.id, "domain admin contact2 password"}
    { }
    ::LibFred::InfoContactData admin_contact1;
    ::LibFred::InfoContactData admin_contact2;
    ::LibFred::InfoDomainData domain;
    HasAuthinfo domain_authinfo;
    HasAuthinfo admin_contact1_authinfo;
    HasAuthinfo admin_contact2_authinfo;
};

struct has_domain_and_a_different_registrar : has_domain
{
    ::LibFred::InfoRegistrarData the_different_registrar;

    has_domain_and_a_different_registrar()
    {
        const std::string diff_reg_handle = "REGISTRAR2";
        ::LibFred::CreateRegistrar{
                diff_reg_handle,
                "Name " + diff_reg_handle,
                "Organization " + diff_reg_handle,
                {"Street " + diff_reg_handle},
                "City " + diff_reg_handle,
                "PostalCode " + diff_reg_handle,
                "Telephone " + diff_reg_handle,
                "Email " + diff_reg_handle,
                "Url " + diff_reg_handle,
                "Dic " + diff_reg_handle}.exec(ctx);
        the_different_registrar = ::LibFred::InfoRegistrarByHandle(diff_reg_handle).exec(ctx).info_registrar_data;
    }
};

struct has_enum_domain_and_a_different_registrar : has_enum_domain
{
    ::LibFred::InfoRegistrarData the_different_registrar;

    has_enum_domain_and_a_different_registrar()
    {
        const std::string diff_reg_handle = "REGISTRAR2";
        ::LibFred::CreateRegistrar{
                diff_reg_handle,
                "Name " + diff_reg_handle,
                "Organization " + diff_reg_handle,
                {"Street " + diff_reg_handle},
                "City " + diff_reg_handle,
                "PostalCode " + diff_reg_handle,
                "Telephone " + diff_reg_handle,
                "Email " + diff_reg_handle,
                "Url " + diff_reg_handle,
                "Dic " + diff_reg_handle}.exec(ctx);
        the_different_registrar = ::LibFred::InfoRegistrarByHandle(diff_reg_handle).exec(ctx).info_registrar_data;
    }
};

struct has_keyset_and_a_different_registrar : has_keyset
{
    ::LibFred::InfoRegistrarData the_different_registrar;

    has_keyset_and_a_different_registrar()
    {
        const std::string diff_reg_handle = "REGISTRAR2";
        ::LibFred::CreateRegistrar{
                diff_reg_handle,
                "Name " + diff_reg_handle,
                "Organization " + diff_reg_handle,
                {"Street " + diff_reg_handle},
                "City " + diff_reg_handle,
                "PostalCode " + diff_reg_handle,
                "Telephone " + diff_reg_handle,
                "Email " + diff_reg_handle,
                "Url " + diff_reg_handle,
                "Dic " + diff_reg_handle}.exec(ctx);
        the_different_registrar = ::LibFred::InfoRegistrarByHandle(diff_reg_handle).exec(ctx).info_registrar_data;
    }
};

struct has_nsset_and_a_different_registrar : has_nsset
{
    ::LibFred::InfoRegistrarData the_different_registrar;

    has_nsset_and_a_different_registrar()
    {
        const std::string diff_reg_handle = "REGISTRAR2";
        ::LibFred::CreateRegistrar{
                diff_reg_handle,
                "Name " + diff_reg_handle,
                "Organization " + diff_reg_handle,
                {"Street " + diff_reg_handle},
                "City " + diff_reg_handle,
                "PostalCode " + diff_reg_handle,
                "Telephone " + diff_reg_handle,
                "Email " + diff_reg_handle,
                "Url " + diff_reg_handle,
                "Dic " + diff_reg_handle}.exec(ctx);
        the_different_registrar = ::LibFred::InfoRegistrarByHandle(diff_reg_handle).exec(ctx).info_registrar_data;
    }
};

}//namespace Test

#endif//UTIL_HH_EF3AC5061B72408083458556AF1CEE43
