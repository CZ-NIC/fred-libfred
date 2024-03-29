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

#include "libfred/object_state/create_admin_object_block_request_id.hh"
#include "libfred/object_state/perform_object_state_request.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrable_object/contact/check_contact.hh"
#include "libfred/registrable_object/contact/copy_contact.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/registrable_object/contact/delete_contact.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/contact/info_contact_diff.hh"
#include "libfred/registrable_object/contact/merge_contact.hh"
#include "libfred/registrable_object/contact/update_contact.hh"
#include "libfred/registrable_object/domain/check_domain.hh"
#include "libfred/registrable_object/domain/create_domain.hh"
#include "libfred/registrable_object/domain/delete_domain.hh"
#include "libfred/registrable_object/domain/info_domain.hh"
#include "libfred/registrable_object/domain/info_domain_diff.hh"
#include "libfred/registrable_object/domain/renew_domain.hh"
#include "libfred/registrable_object/domain/update_domain.hh"

#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"

#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <string>

const std::string server_name = "test-create-administrative-object-block-request-id";

struct create_administrative_object_block_request_id_fixture : public Test::instantiate_db_template
{
    std::string registrar_handle;
    std::string xmark;
    std::string admin_contact2_handle;
    std::string registrant_contact_handle;
    std::string test_domain_fqdn;
    ::LibFred::ObjectId test_domain_id;
    ::LibFred::StatusList status_list;

    create_administrative_object_block_request_id_fixture()
    :   xmark(Random::Generator().get_seq(Random::CharSet::digits(), 6)),
        admin_contact2_handle(std::string("TEST-OSR-ADMIN-CONTACT-HANDLE") + xmark),
        registrant_contact_handle(std::string("TEST-OSR-REGISTRANT-CONTACT-HANDLE") + xmark),
        test_domain_fqdn ( std::string("fred")+xmark+".cz")
    {
        ::LibFred::OperationContextCreator ctx;
        registrar_handle = static_cast<std::string>(ctx.get_conn().exec(
            "SELECT handle FROM registrar WHERE system ORDER BY id LIMIT 1")[0][0]);
        BOOST_CHECK(!registrar_handle.empty());//expecting existing system registrar

        ::LibFred::Contact::PlaceAddress place;
        place.street1 = std::string("STR1") + xmark;
        place.city = "Praha";
        place.postalcode = "11150";
        place.country = "CZ";
        ::LibFred::CreateContact(admin_contact2_handle, registrar_handle)
            .set_name(std::string("TEST-AOB-ADMIN-CONTACT NAME")+xmark)
            .set_disclosename(true)
            .set_place(place)
            .set_discloseaddress(true)
            .exec(ctx);

        ::LibFred::CreateContact(registrant_contact_handle, registrar_handle)
                .set_name(std::string("TEST-REGISTRANT-CONTACT NAME")+xmark)
                .set_disclosename(true)
                .set_place(place)
                .set_discloseaddress(true)
                .exec(ctx);

        ::LibFred::CreateDomain(
                test_domain_fqdn //const std::string& fqdn
                , registrar_handle //const std::string& registrar
                , registrant_contact_handle //registrant
                )
        .set_admin_contacts(Util::vector_of<std::string>(admin_contact2_handle))
        .exec(ctx);

        const Database::Result status_result = ctx.get_conn().exec("SELECT name FROM enum_object_states WHERE manual AND 3=ANY(types) AND name!='serverBlocked'");
        for (::size_t idx = 0; idx < status_result.size(); ++idx) {
            status_list.insert(status_result[idx][0]);
        }
        test_domain_id = static_cast< ::LibFred::ObjectId >(ctx.get_conn().exec_params(
            "SELECT id "
            "FROM object_registry "
            "WHERE name=$1::text AND "
                  "type=3 AND "
                  "erdate IS NULL", Database::query_param_list(test_domain_fqdn))[0][0]);
        ctx.commit_transaction();
    }
    ~create_administrative_object_block_request_id_fixture()
    {}
};

BOOST_FIXTURE_TEST_SUITE(TestCreateAdminObjectBlockRequestId, create_administrative_object_block_request_id_fixture )

/**
 * test CreateAdminObjectBlockRequestId
 * ...
 * calls in test shouldn't throw
 */
BOOST_AUTO_TEST_CASE(create_administrative_object_block_request_id)
{
    {
        ::LibFred::OperationContextCreator ctx;
        const std::string handle = ::LibFred::CreateAdminObjectBlockRequestId(test_domain_id, status_list).exec(ctx);
        BOOST_CHECK(handle == test_domain_fqdn);
        ctx.commit_transaction();
    }
    ::LibFred::OperationContextCreator ctx;
    const Database::Result status_result = ctx.get_conn().exec_params(
        "SELECT eos.name "
        "FROM object_state_request osr "
        "JOIN object_registry obr ON obr.id=osr.object_id "
        "JOIN enum_object_states eos ON (eos.id=osr.state_id AND obr.type=ANY(eos.types)) "
        "WHERE osr.object_id=$1::bigint AND "
              "osr.valid_from<=CURRENT_TIMESTAMP AND "
              "(osr.valid_to IS NULL OR CURRENT_TIMESTAMP<valid_to) AND "
              "obr.erdate IS NULL AND "
              "osr.canceled IS NULL AND "
              "eos.manual", Database::query_param_list(test_domain_id));
    BOOST_CHECK((status_list.size() + 1) <= status_result.size()); // status_list + 'serverBlocked'
    ::LibFred::StatusList domain_status_list;
    for (::size_t idx = 0; idx < status_result.size(); ++idx) {
        domain_status_list.insert(static_cast< std::string >(status_result[idx][0]));
    }
    BOOST_CHECK(domain_status_list.find("serverBlocked") != domain_status_list.end());
    for (::LibFred::StatusList::const_iterator pStatus = status_list.begin(); pStatus != status_list.end(); ++pStatus) {
        BOOST_CHECK(domain_status_list.find(*pStatus) != domain_status_list.end());
    }
}

/**
 * test CreateAdminObjectBlockRequestIdBad
 * ...
 * calls in test shouldn't throw
 */
BOOST_AUTO_TEST_CASE(create_administrative_object_block_request_id_bad)
{
    ::LibFred::StatusList bad_status_list = status_list;
    BOOST_CHECK_EXCEPTION(
            [&]()
            {
                ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
                const auto status_result = ctx.get_conn().exec(
                        "SELECT name "
                        "FROM enum_object_states "
                        "WHERE NOT (manual AND 3 = ANY(types)) AND "
                              "name NOT LIKE 'server%'");
                for (::size_t idx = 0; idx < status_result.size(); ++idx)
                {
                    bad_status_list.insert(status_result[idx][0]);
                }
                bad_status_list.insert("BadStatus" + xmark);
                ::LibFred::CreateAdminObjectBlockRequestId(test_domain_id, bad_status_list).exec(ctx);
            }(),
            ::LibFred::CreateAdminObjectBlockRequestId::Exception,
            [&](auto&& e)
            {
                BOOST_CHECK(e.is_set_vector_of_state_not_found());
                BOOST_CHECK_EQUAL(e.get_vector_of_state_not_found().size(), bad_status_list.size() - status_list.size());
                return e.is_set_vector_of_state_not_found() &&
                       (e.get_vector_of_state_not_found().size() == (bad_status_list.size() - status_list.size()));
            });

    ::LibFred::StatusList status_list_a;
    ::LibFred::StatusList status_list_b = status_list;
    status_list_a.insert(*status_list_b.begin());
    status_list_b.erase(status_list_b.begin());
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::CreateAdminObjectBlockRequestId(test_domain_id, status_list_a).exec(ctx);
        ::LibFred::PerformObjectStateRequest(test_domain_id).exec(ctx);
        ctx.commit_transaction();
    }
    try {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::CreateAdminObjectBlockRequestId(test_domain_id, status_list_b).exec(ctx);
        ctx.commit_transaction();
        BOOST_CHECK(false);
    }
    catch (const ::LibFred::CreateAdminObjectBlockRequestId::Exception &ex) {
        BOOST_CHECK(ex.is_set_server_blocked_present());
        BOOST_CHECK(ex.get_server_blocked_present() == test_domain_id);
    }
}

BOOST_AUTO_TEST_SUITE_END()//TestCreateAdminObjectBlockRequestId
