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
#include "libfred/registrable_object/domain/info_domain_impl.hh"
#include "libfred/registrable_object/domain/renew_domain.hh"
#include "libfred/registrable_object/domain/update_domain.hh"
#include "libfred/registrable_object/nsset/check_nsset.hh"
#include "libfred/registrable_object/nsset/create_nsset.hh"
#include "libfred/registrable_object/nsset/delete_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset_diff.hh"
#include "libfred/registrable_object/nsset/update_nsset.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <string>

const std::string server_name = "test-delete-nsset";


struct delete_nsset_fixture : public Test::instantiate_db_template
{
    std::string registrar_handle;
    std::string xmark;
    std::string admin_contact_handle;
    std::string test_nsset_handle;
    std::string test_domain_fqdn;

    delete_nsset_fixture()
    :xmark(Random::Generator().get_seq(Random::CharSet::digits(), 6))
    , admin_contact_handle(std::string("TEST-ADMIN-CONTACT3-HANDLE")+xmark)
    , test_nsset_handle ( std::string("TEST-DEL-NSSET-")+xmark+"-HANDLE")
    , test_domain_fqdn ( std::string("fred")+xmark+".cz")
    {
        ::LibFred::OperationContextCreator ctx;
        registrar_handle = static_cast<std::string>(ctx.get_conn().exec(
            "SELECT handle FROM registrar WHERE system = TRUE ORDER BY id LIMIT 1")[0][0]);
        BOOST_CHECK(!registrar_handle.empty());//expecting existing system registrar

        ::LibFred::Contact::PlaceAddress place;
        place.street1 = std::string("STR1") + xmark;
        place.city = "Praha";
        place.postalcode = "11150";
        place.country = "CZ";
        ::LibFred::CreateContact(admin_contact_handle, registrar_handle)
            .set_name(std::string("TEST-ADMIN-CONTACT3 NAME")+xmark)
            .set_disclosename(true)
            .set_place(place)
            .set_discloseaddress(true)
            .exec(ctx);

        ::LibFred::CreateNsset(test_nsset_handle, registrar_handle)
                .set_tech_contacts(Util::vector_of<std::string>(admin_contact_handle))
                .exec(ctx);

        ctx.commit_transaction();
    }
    ~delete_nsset_fixture()
    {}
};

BOOST_FIXTURE_TEST_SUITE(TestDeleteNsset, delete_nsset_fixture)

/**
 * test DeleteNsset
 * create test nsset, delete test nsset, check erdate of test nsset is null
 * calls in test shouldn't throw
 */
BOOST_AUTO_TEST_CASE(delete_nsset)
{
    ::LibFred::OperationContextCreator ctx;

    ::LibFred::InfoNssetOutput nsset_info1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    BOOST_CHECK(nsset_info1.info_nsset_data.delete_time.isnull());

    ::LibFred::DeleteNssetByHandle(test_nsset_handle).exec(ctx);

    std::vector<::LibFred::InfoNssetOutput> nsset_history_info1 = ::LibFred::InfoNssetHistoryByRoid(
    nsset_info1.info_nsset_data.roid).exec(ctx);

    BOOST_CHECK(!nsset_history_info1.at(0).info_nsset_data.delete_time.isnull());

    ::LibFred::InfoNssetOutput nsset_info1_with_change = nsset_info1;
    nsset_info1_with_change.info_nsset_data.delete_time = nsset_history_info1.at(0).info_nsset_data.delete_time;

    BOOST_CHECK(nsset_info1_with_change == nsset_history_info1.at(0));

    BOOST_CHECK(!nsset_history_info1.at(0).info_nsset_data.delete_time.isnull());

    BOOST_CHECK(nsset_history_info1.at(0).next_historyid.isnull());
    BOOST_CHECK(!nsset_history_info1.at(0).history_valid_from.is_not_a_date_time());
    BOOST_CHECK(!nsset_history_info1.at(0).history_valid_to.isnull());
    BOOST_CHECK(nsset_history_info1.at(0).history_valid_from <= nsset_history_info1.at(0).history_valid_to.get_value());

    BOOST_CHECK(static_cast<bool>(ctx.get_conn().exec_params(
        "select erdate is not null from object_registry where name = $1::text"
        , Database::query_param_list(test_nsset_handle))[0][0]));

    BOOST_CHECK(ctx.get_conn().exec_params(
        "SELECT o.id FROM object o JOIN object_registry oreg ON o.id = oreg.id WHERE oreg.name = $1::text"
        , Database::query_param_list(test_nsset_handle)).size() == 0);

    BOOST_CHECK(ctx.get_conn().exec_params(
        "SELECT k.id FROM nsset k JOIN object_registry oreg ON k.id = oreg.id WHERE oreg.name = $1::text"
        , Database::query_param_list(test_nsset_handle)).size() == 0);

    BOOST_CHECK(ctx.get_conn().exec_params(
        "SELECT ncm.contactid FROM nsset_contact_map ncm JOIN object_registry oreg ON ncm.nssetid = oreg.id WHERE oreg.name = $1::text"
        , Database::query_param_list(test_nsset_handle)).size() == 0);

    ctx.commit_transaction();
}//delete_nsset


/**
 * test DeleteNsset with wrong handle
 */

BOOST_AUTO_TEST_CASE(delete_nsset_with_wrong_handle)
{
    std::string bad_test_nsset_handle = std::string("bad")+test_nsset_handle;
    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::DeleteNssetByHandle(bad_test_nsset_handle).exec(ctx);
        ctx.commit_transaction();
    }
    catch (const ::LibFred::DeleteNssetByHandle::Exception& ex)
    {
        BOOST_CHECK(ex.is_set_unknown_nsset_handle());
        BOOST_CHECK(ex.get_unknown_nsset_handle().compare(bad_test_nsset_handle) == 0);
    }
}

/**
 * test DeleteNsset linked
 */

BOOST_AUTO_TEST_CASE(delete_linked_nsset)
{
    {
        ::LibFred::OperationContextCreator ctx;
        //create linked object

        ::LibFred::CreateDomain(test_domain_fqdn //const std::string& fqdn
            , registrar_handle //const std::string& registrar
            , admin_contact_handle //registrant
            ).set_admin_contacts(Util::vector_of<std::string>(admin_contact_handle))
            .set_nsset(test_nsset_handle)
            .exec(ctx);

       ctx.commit_transaction();
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::DeleteNssetByHandle(test_nsset_handle).exec(ctx);
        ctx.commit_transaction();
    }
    catch (const ::LibFred::DeleteNssetByHandle::Exception& ex)
    {
        BOOST_CHECK(ex.is_set_object_linked_to_nsset_handle());
        BOOST_CHECK(ex.get_object_linked_to_nsset_handle().compare(test_nsset_handle) == 0);
    }
}


BOOST_AUTO_TEST_SUITE_END();//TestDeleteNsset
