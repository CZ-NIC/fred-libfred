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

#include "libfred/object/check_authinfo.hh"
#include "libfred/opcontext.hh"
#include "libfred/registrable_object/contact/check_contact.hh"
#include "libfred/registrable_object/contact/copy_contact.hh"
#include "libfred/registrable_object/contact/create_contact.hh"
#include "libfred/registrable_object/contact/delete_contact.hh"
#include "libfred/registrable_object/contact/info_contact.hh"
#include "libfred/registrable_object/contact/info_contact_diff.hh"
#include "libfred/registrable_object/contact/merge_contact.hh"
#include "libfred/registrable_object/contact/update_contact.hh"
#include "libfred/registrable_object/nsset/check_nsset.hh"
#include "libfred/registrable_object/nsset/create_nsset.hh"
#include "libfred/registrable_object/nsset/delete_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset.hh"
#include "libfred/registrable_object/nsset/info_nsset_diff.hh"
#include "libfred/registrable_object/nsset/info_nsset_impl.hh"
#include "libfred/registrable_object/nsset/update_nsset.hh"
#include "util/random/char_set/char_set.hh"
#include "util/random/random.hh"
#include "test/setup/fixtures.hh"

#include <boost/test/unit_test.hpp>

#include <string>

BOOST_FIXTURE_TEST_SUITE(TestUpdateNsset, Test::instantiate_db_template)

const std::string server_name = "test-update-nsset";

/**
 * test call InfoNssetByHandle
*/

BOOST_AUTO_TEST_CASE(info_nsset)
{
    namespace ip = boost::asio::ip;
    ::LibFred::OperationContextCreator ctx;

    const std::string registrar_handle = static_cast<std::string>(
            ctx.get_conn().exec("SELECT handle FROM registrar WHERE system ORDER BY id LIMIT 1")[0][0]);
    BOOST_CHECK(!registrar_handle.empty());//expecting existing system registrar

    const std::string xmark = Random::Generator().get_seq(Random::CharSet::digits(), 6);

    const std::string admin_contact2_handle = "TEST-ADMIN-CONTACT2-HANDLE" + xmark;
    ::LibFred::Contact::PlaceAddress place;
    place.street1 = "STR1" + xmark;
    place.city = "Praha";
    place.postalcode = "11150";
    place.country = "CZ";
    ::LibFred::CreateContact(admin_contact2_handle, registrar_handle)
        .set_name("TEST-ADMIN-CONTACT2 NAME" + xmark)
        .set_disclosename(true)
        .set_place(place)
        .set_discloseaddress(true)
        .exec(ctx);

    const std::string admin_contact3_handle = "TEST-ADMIN-CONTACT3-HANDLE" + xmark;
    ::LibFred::CreateContact(admin_contact3_handle, registrar_handle)
        .set_name("TEST-ADMIN-CONTACT3 NAME" + xmark)
        .set_disclosename(true)
        .set_place(place)
        .set_discloseaddress(true)
        .exec(ctx);

    const std::string test_nsset_handle = "TEST-NSSET-HANDLE" + xmark;
    ::LibFred::CreateNsset(test_nsset_handle, registrar_handle)
        .set_dns_hosts(
                {
                    ::LibFred::DnsHost("a.ns.nic.cz", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }),//add_dns
                    ::LibFred::DnsHost("b.ns.nic.cz", { ip::address::from_string("127.0.0.4"), ip::address::from_string("127.1.1.4") }) //add_dns
                })
        .exec(ctx);

    //ctx.commit_transaction();

    const ::LibFred::InfoNssetOutput nsset_info1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const ::LibFred::InfoNssetOutput nsset_info2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).set_lock().exec(ctx);
}

struct update_nsset_fixture : Test::instantiate_db_template
{
    std::string registrar_handle;
    std::string xmark;
    std::string admin_contact2_handle;
    std::string admin_contact3_handle;
    std::string test_nsset_handle;

    update_nsset_fixture()
        : xmark(Random::Generator().get_seq(Random::CharSet::digits(), 6)),
          admin_contact2_handle("TEST-ADMIN-CONTACT2-HANDLE" + xmark),
          admin_contact3_handle("TEST-ADMIN-CONTACT3-HANDLE" + xmark),
          test_nsset_handle("TEST-NSSET-HANDLE" + xmark)
    {
        namespace ip = boost::asio::ip;
        ::LibFred::OperationContextCreator ctx;
        registrar_handle = static_cast<std::string>(ctx.get_conn().exec(
            "SELECT handle FROM registrar WHERE system ORDER BY id LIMIT 1")[0][0]);
        BOOST_CHECK(!registrar_handle.empty());//expecting existing system registrar

        ::LibFred::Contact::PlaceAddress place;
        place.street1 = "STR1" + xmark;
        place.city = "Praha";
        place.postalcode = "11150";
        place.country = "CZ";
        ::LibFred::CreateContact(admin_contact2_handle, registrar_handle)
            .set_name("TEST-ADMIN-CONTACT2 NAME" + xmark)
            .set_disclosename(true)
            .set_place(place)
            .set_discloseaddress(true)
            .exec(ctx);

        ::LibFred::CreateContact(admin_contact3_handle, registrar_handle)
            .set_name("TEST-ADMIN-CONTACT3 NAME" + xmark)
            .set_disclosename(true)
            .set_place(place)
            .set_discloseaddress(true)
            .exec(ctx);

        ::LibFred::CreateNsset(test_nsset_handle, registrar_handle)
            .set_dns_hosts(
                    {
                        ::LibFred::DnsHost("a.ns.nic.cz", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }),//add_dns
                        ::LibFred::DnsHost("b.ns.nic.cz", { ip::address::from_string("127.0.0.4"), ip::address::from_string("127.1.1.4") }) //add_dns
                    })
            .set_tech_contacts(Util::vector_of<std::string>(admin_contact3_handle))
            .exec(ctx);

        ctx.commit_transaction();
    }
    ~update_nsset_fixture()
    {}
};

/**
 * test UpdateNsset
 * test UpdateNsset construction and methods calls with precreated data
 * calls in test shouldn't throw
 */
BOOST_FIXTURE_TEST_CASE(update_nsset, update_nsset_fixture)
{
    namespace ip = boost::asio::ip;

    ::LibFred::OperationContextCreator ctx;
    const ::LibFred::InfoNssetOutput info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_1 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    //update_registrar_handle check
    BOOST_CHECK(info_data_1.info_nsset_data.update_registrar_handle.isnull());

    //update_time
    BOOST_CHECK(info_data_1.info_nsset_data.update_time.isnull());

    //history check
    BOOST_CHECK(history_info_data_1.at(0) == info_data_1);
    BOOST_CHECK_EQUAL(history_info_data_1.at(0).info_nsset_data.crhistoryid, info_data_1.info_nsset_data.historyid);

    //empty update
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_2 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_1_with_changes = info_data_1;

    //updated historyid
    BOOST_CHECK_NE(info_data_1.info_nsset_data.historyid, info_data_2.info_nsset_data.historyid);
    info_data_1_with_changes.info_nsset_data.historyid = info_data_2.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_1.info_nsset_data.history_uuid), get_raw_value_from(info_data_2.info_nsset_data.history_uuid));
    info_data_1_with_changes.info_nsset_data.history_uuid = info_data_2.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_2.info_nsset_data.update_registrar_handle.get_value());
    info_data_1_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_1_with_changes.info_nsset_data.update_time = info_data_2.info_nsset_data.update_time;

    //check changes made by last update
    BOOST_CHECK(info_data_1_with_changes == info_data_2);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_2.at(0) == info_data_2);
    BOOST_CHECK(history_info_data_2.at(1) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_2.at(1).info_nsset_data == history_info_data_1.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK(history_info_data_2.at(1).next_historyid.get_value() == history_info_data_2.at(0).info_nsset_data.historyid);
    BOOST_CHECK(history_info_data_2.at(0).info_nsset_data.crhistoryid == info_data_2.info_nsset_data.crhistoryid);

    ::LibFred::UpdateNsset(
            test_nsset_handle,//handle
            registrar_handle,//registrar
            Optional<std::string>(),//authinfo
            std::vector<::LibFred::DnsHost>(),//add_dns
            std::vector<std::string>(),//rem_dns
            std::vector<std::string>(),//add_tech_contact
            std::vector<std::string>(),//rem_tech_contact
            Optional<short>(),//tech_check_level
            Optional<unsigned long long>())//logd_request_id
        .exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_3 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_3 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_2_with_changes = info_data_2;

    //updated historyid
    BOOST_CHECK_NE(info_data_2.info_nsset_data.historyid, info_data_3.info_nsset_data.historyid);
    info_data_2_with_changes.info_nsset_data.historyid = info_data_3.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_2.info_nsset_data.history_uuid), get_raw_value_from(info_data_3.info_nsset_data.history_uuid));
    info_data_2_with_changes.info_nsset_data.history_uuid = info_data_3.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_3.info_nsset_data.update_registrar_handle.get_value());
    info_data_2_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_2_with_changes.info_nsset_data.update_time = info_data_3.info_nsset_data.update_time;

    //check changes made by last update
    BOOST_CHECK(info_data_2_with_changes == info_data_3);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_3.at(0) == info_data_3);
    BOOST_CHECK(history_info_data_3.at(1) == info_data_2);
    BOOST_CHECK(history_info_data_3.at(2) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_3.at(1).info_nsset_data == history_info_data_2.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_3.at(1).next_historyid.get_value(), history_info_data_3.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_3.at(0).info_nsset_data.crhistoryid, info_data_3.info_nsset_data.crhistoryid);

    ::LibFred::UpdateNsset(
            test_nsset_handle,//handle
            registrar_handle,//registrar
            "passwd",//authinfo
            {
                ::LibFred::DnsHost("host", { ip::address::from_string("127.0.0.1"), ip::address::from_string("127.1.1.1") }),//add_dns
                ::LibFred::DnsHost("host1", { ip::address::from_string("127.0.0.2"), ip::address::from_string("127.1.1.2") }),//add_dns
                ::LibFred::DnsHost("b.ns.nic.cz", { ip::address::from_string("127.0.0.4") })//add_dns
            },
            { "a.ns.nic.cz", "b.ns.nic.cz" },//rem_dns
            { admin_contact2_handle },//std::vector<std::string>() //add_tech_contact
            { admin_contact2_handle },//std::vector<std::string>() //rem_tech_contact
            0,//tech_check_level
            0)//logd_request_id
        .exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_4 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_4 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_3_with_changes = info_data_3;

    //updated historyid
    BOOST_CHECK_NE(info_data_3.info_nsset_data.historyid, info_data_4.info_nsset_data.historyid);
    info_data_3_with_changes.info_nsset_data.historyid = info_data_4.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_3.info_nsset_data.history_uuid), get_raw_value_from(info_data_4.info_nsset_data.history_uuid));
    info_data_3_with_changes.info_nsset_data.history_uuid = info_data_4.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_4.info_nsset_data.update_registrar_handle.get_value());
    info_data_3_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated sponsoring_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_4.info_nsset_data.sponsoring_registrar_handle);
    info_data_3_with_changes.info_nsset_data.sponsoring_registrar_handle = registrar_handle;

    //updated update_time
    info_data_3_with_changes.info_nsset_data.update_time = info_data_4.info_nsset_data.update_time;

    //updated authinfopw
    BOOST_CHECK_EQUAL(
            ::LibFred::Object::CheckAuthinfo{::LibFred::Object::ObjectId{info_data_4.info_nsset_data.id}}
                    .exec(ctx, "passwd", ::LibFred::Object::CheckAuthinfo::increment_usage),
            1);

    //update dns_hosts
    info_data_3_with_changes.info_nsset_data.dns_hosts =
            {
                ::LibFred::DnsHost("b.ns.nic.cz", { ip::address::from_string("127.0.0.4") }),
                ::LibFred::DnsHost("host", { ip::address::from_string("127.0.0.1"), ip::address::from_string("127.1.1.1") }),
                ::LibFred::DnsHost("host1", { ip::address::from_string("127.0.0.2"), ip::address::from_string("127.1.1.2") })
            };

    //check changes made by last update
    BOOST_CHECK(info_data_3_with_changes == info_data_4);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_4.at(0) == info_data_4);
    BOOST_CHECK(history_info_data_4.at(1) == info_data_3);
    BOOST_CHECK(history_info_data_4.at(2) == info_data_2);
    BOOST_CHECK(history_info_data_4.at(3) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_4.at(1).info_nsset_data == history_info_data_3.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_4.at(1).next_historyid.get_value(), history_info_data_4.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_4.at(0).info_nsset_data.crhistoryid, info_data_4.info_nsset_data.crhistoryid);

    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .add_dns(::LibFred::DnsHost("host2", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }))
        .rem_dns("b.ns.nic.cz")
        .add_tech_contact(admin_contact2_handle)
        .rem_tech_contact(admin_contact2_handle)
        .set_authinfo("passw")
        .set_logd_request_id(4)
        .set_tech_check_level(3)
        .exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_5 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_5 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_4_with_changes = info_data_4;

    //updated historyid
    BOOST_CHECK_NE(info_data_4.info_nsset_data.historyid, info_data_5.info_nsset_data.historyid);
    info_data_4_with_changes.info_nsset_data.historyid = info_data_5.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_4.info_nsset_data.history_uuid), get_raw_value_from(info_data_5.info_nsset_data.history_uuid));
    info_data_4_with_changes.info_nsset_data.history_uuid = info_data_5.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_5.info_nsset_data.update_registrar_handle.get_value());
    info_data_4_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated sponsoring_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_5.info_nsset_data.sponsoring_registrar_handle);
    info_data_4_with_changes.info_nsset_data.sponsoring_registrar_handle = registrar_handle;

    //updated update_time
    info_data_4_with_changes.info_nsset_data.update_time = info_data_5.info_nsset_data.update_time;

    //updated authinfopw
    BOOST_CHECK_EQUAL(
            ::LibFred::Object::CheckAuthinfo{::LibFred::Object::ObjectId{info_data_4.info_nsset_data.id}}
                    .exec(ctx, "passwd", ::LibFred::Object::CheckAuthinfo::increment_usage),
            0);
    BOOST_CHECK_EQUAL(
            ::LibFred::Object::CheckAuthinfo{::LibFred::Object::ObjectId{info_data_5.info_nsset_data.id}}
                    .exec(ctx, "passw", ::LibFred::Object::CheckAuthinfo::increment_usage),
            1);

    //update dns_hosts
    info_data_4_with_changes.info_nsset_data.dns_hosts =
            {
                ::LibFred::DnsHost("host2", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }),
                ::LibFred::DnsHost("host", { ip::address::from_string("127.0.0.1"), ip::address::from_string("127.1.1.1") }),
                ::LibFred::DnsHost("host1", { ip::address::from_string("127.0.0.2"), ip::address::from_string("127.1.1.2") })
            };

    //updated tech_check_level
    BOOST_CHECK_EQUAL(info_data_5.info_nsset_data.tech_check_level.get_value(), 3);
    info_data_4_with_changes.info_nsset_data.tech_check_level = 3;

    //check logd request_id
    BOOST_CHECK_EQUAL(history_info_data_5.at(0).logd_request_id.get_value(), 4);

    //check changes made by last update
    BOOST_CHECK(info_data_4_with_changes == info_data_5);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_5.at(0) == info_data_5);
    BOOST_CHECK(history_info_data_5.at(1) == info_data_4);
    BOOST_CHECK(history_info_data_5.at(2) == info_data_3);
    BOOST_CHECK(history_info_data_5.at(3) == info_data_2);
    BOOST_CHECK(history_info_data_5.at(4) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_5.at(1).info_nsset_data == history_info_data_4.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_5.at(1).next_historyid.get_value(), history_info_data_5.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_5.at(0).info_nsset_data.crhistoryid, info_data_5.info_nsset_data.crhistoryid);

    //add dns host
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).add_dns(::LibFred::DnsHost("host3", { ip::address::from_string("127.0.0.5"), ip::address::from_string("127.1.1.5") })).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_6 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_6 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_5_with_changes = info_data_5;

    //updated historyid
    BOOST_CHECK_NE(info_data_5.info_nsset_data.historyid, info_data_6.info_nsset_data.historyid);
    info_data_5_with_changes.info_nsset_data.historyid = info_data_6.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_5.info_nsset_data.history_uuid), get_raw_value_from(info_data_6.info_nsset_data.history_uuid));
    info_data_5_with_changes.info_nsset_data.history_uuid = info_data_6.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_6.info_nsset_data.update_registrar_handle.get_value());
    info_data_5_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_5_with_changes.info_nsset_data.update_time = info_data_6.info_nsset_data.update_time;

    //update dns_hosts
    info_data_5_with_changes.info_nsset_data.dns_hosts =
            {
                ::LibFred::DnsHost("host2", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }),
                ::LibFred::DnsHost("host", { ip::address::from_string("127.0.0.1"), ip::address::from_string("127.1.1.1") }),
                ::LibFred::DnsHost("host1", { ip::address::from_string("127.0.0.2"), ip::address::from_string("127.1.1.2") }),
                ::LibFred::DnsHost("host3", { ip::address::from_string("127.0.0.5"), ip::address::from_string("127.1.1.5") })
            };

    //check changes made by last update
    BOOST_CHECK(info_data_5_with_changes == info_data_6);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_6.at(0) == info_data_6);
    BOOST_CHECK(history_info_data_6.at(1) == info_data_5);
    BOOST_CHECK(history_info_data_6.at(2) == info_data_4);
    BOOST_CHECK(history_info_data_6.at(3) == info_data_3);
    BOOST_CHECK(history_info_data_6.at(4) == info_data_2);
    BOOST_CHECK(history_info_data_6.at(5) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_6.at(1).info_nsset_data == history_info_data_5.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_6.at(1).next_historyid.get_value(), history_info_data_6.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_6.at(0).info_nsset_data.crhistoryid, info_data_6.info_nsset_data.crhistoryid);

    //rem dns host
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).rem_dns("host2").exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_7 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_7 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_6_with_changes = info_data_6;

    //updated historyid
    BOOST_CHECK_NE(info_data_6.info_nsset_data.historyid, info_data_7.info_nsset_data.historyid);
    info_data_6_with_changes.info_nsset_data.historyid = info_data_7.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_6.info_nsset_data.history_uuid), get_raw_value_from(info_data_7.info_nsset_data.history_uuid));
    info_data_6_with_changes.info_nsset_data.history_uuid = info_data_7.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_7.info_nsset_data.update_registrar_handle.get_value());
    info_data_6_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_6_with_changes.info_nsset_data.update_time = info_data_7.info_nsset_data.update_time;

    //update dns_hosts
    info_data_6_with_changes.info_nsset_data.dns_hosts =
            {
                ::LibFred::DnsHost("host", { ip::address::from_string("127.0.0.1"), ip::address::from_string("127.1.1.1") }),
                ::LibFred::DnsHost("host1", { ip::address::from_string("127.0.0.2"), ip::address::from_string("127.1.1.2") }),
                ::LibFred::DnsHost("host3", { ip::address::from_string("127.0.0.5"), ip::address::from_string("127.1.1.5") })
            };
    //check changes made by last update
    BOOST_CHECK(info_data_6_with_changes == info_data_7);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_7.at(0) == info_data_7);
    BOOST_CHECK(history_info_data_7.at(1) == info_data_6);
    BOOST_CHECK(history_info_data_7.at(2) == info_data_5);
    BOOST_CHECK(history_info_data_7.at(3) == info_data_4);
    BOOST_CHECK(history_info_data_7.at(4) == info_data_3);
    BOOST_CHECK(history_info_data_7.at(5) == info_data_2);
    BOOST_CHECK(history_info_data_7.at(6) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_7.at(1).info_nsset_data == history_info_data_6.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_7.at(1).next_historyid.get_value(), history_info_data_7.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_7.at(0).info_nsset_data.crhistoryid, info_data_7.info_nsset_data.crhistoryid);

    //rem tech contact
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).rem_tech_contact(admin_contact3_handle).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_8 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_8 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_7_with_changes = info_data_7;

    //updated historyid
    BOOST_CHECK_NE(info_data_7.info_nsset_data.historyid, info_data_8.info_nsset_data.historyid);
    info_data_7_with_changes.info_nsset_data.historyid = info_data_8.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_7.info_nsset_data.history_uuid), get_raw_value_from(info_data_8.info_nsset_data.history_uuid));
    info_data_7_with_changes.info_nsset_data.history_uuid = info_data_8.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK(registrar_handle == info_data_8.info_nsset_data.update_registrar_handle.get_value());
    info_data_7_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_7_with_changes.info_nsset_data.update_time = info_data_8.info_nsset_data.update_time;

    ::LibFred::InfoContactOutput admin_contact3_info  = ::LibFred::InfoContactByHandle(admin_contact3_handle).exec(ctx);
    //rem tech contact
    info_data_7_with_changes.info_nsset_data.tech_contacts.erase(
            std::remove(
                    info_data_7_with_changes.info_nsset_data.tech_contacts.begin(),
                    info_data_7_with_changes.info_nsset_data.tech_contacts.end(),
                    ::LibFred::RegistrableObject::Contact::ContactReference(
                            admin_contact3_info.info_contact_data.id,
                            admin_contact3_info.info_contact_data.handle,
                            admin_contact3_info.info_contact_data.uuid)));

    //check changes made by last update
    BOOST_CHECK(info_data_7_with_changes == info_data_8);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_8.at(0) == info_data_8);
    BOOST_CHECK(history_info_data_8.at(1) == info_data_7);
    BOOST_CHECK(history_info_data_8.at(2) == info_data_6);
    BOOST_CHECK(history_info_data_8.at(3) == info_data_5);
    BOOST_CHECK(history_info_data_8.at(4) == info_data_4);
    BOOST_CHECK(history_info_data_8.at(5) == info_data_3);
    BOOST_CHECK(history_info_data_8.at(6) == info_data_2);
    BOOST_CHECK(history_info_data_8.at(7) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_8.at(1).info_nsset_data == history_info_data_7.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_8.at(1).next_historyid.get_value(), history_info_data_8.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_8.at(0).info_nsset_data.crhistoryid, info_data_8.info_nsset_data.crhistoryid);

    //add tech contact
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).add_tech_contact(admin_contact3_handle).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_9 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_9 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_8_with_changes = info_data_8;

    //updated historyid
    BOOST_CHECK_NE(info_data_8.info_nsset_data.historyid, info_data_9.info_nsset_data.historyid);
    info_data_8_with_changes.info_nsset_data.historyid = info_data_9.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_8.info_nsset_data.history_uuid), get_raw_value_from(info_data_9.info_nsset_data.history_uuid));
    info_data_8_with_changes.info_nsset_data.history_uuid = info_data_9.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_9.info_nsset_data.update_registrar_handle.get_value());
    info_data_8_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_8_with_changes.info_nsset_data.update_time = info_data_9.info_nsset_data.update_time;

    //add tech contact
    info_data_8_with_changes.info_nsset_data.tech_contacts.push_back(
            ::LibFred::RegistrableObject::Contact::ContactReference(
                    admin_contact3_info.info_contact_data.id,
                    admin_contact3_info.info_contact_data.handle,
                    admin_contact3_info.info_contact_data.uuid));

    //check changes made by last update
    BOOST_CHECK(info_data_8_with_changes == info_data_9);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_9.at(0) == info_data_9);
    BOOST_CHECK(history_info_data_9.at(1) == info_data_8);
    BOOST_CHECK(history_info_data_9.at(2) == info_data_7);
    BOOST_CHECK(history_info_data_9.at(3) == info_data_6);
    BOOST_CHECK(history_info_data_9.at(4) == info_data_5);
    BOOST_CHECK(history_info_data_9.at(5) == info_data_4);
    BOOST_CHECK(history_info_data_9.at(6) == info_data_3);
    BOOST_CHECK(history_info_data_9.at(7) == info_data_2);
    BOOST_CHECK(history_info_data_9.at(8) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_9.at(1).info_nsset_data == history_info_data_8.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_9.at(1).next_historyid.get_value(), history_info_data_9.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_9.at(0).info_nsset_data.crhistoryid, info_data_9.info_nsset_data.crhistoryid);

    //set authinfopw
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).set_authinfo("passw").exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_10 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_10 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_9_with_changes = info_data_9;

    //updated historyid
    BOOST_CHECK_NE(info_data_9.info_nsset_data.historyid, info_data_10.info_nsset_data.historyid);
    info_data_9_with_changes.info_nsset_data.historyid = info_data_10.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_9.info_nsset_data.history_uuid), get_raw_value_from(info_data_10.info_nsset_data.history_uuid));
    info_data_9_with_changes.info_nsset_data.history_uuid = info_data_10.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_10.info_nsset_data.update_registrar_handle.get_value());
    info_data_9_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_9_with_changes.info_nsset_data.update_time = info_data_10.info_nsset_data.update_time;

    //check changes made by last update
    BOOST_CHECK(info_data_9_with_changes == info_data_10);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_10.at(0) == info_data_10);
    BOOST_CHECK(history_info_data_10.at(1) == info_data_9);
    BOOST_CHECK(history_info_data_10.at(2) == info_data_8);
    BOOST_CHECK(history_info_data_10.at(3) == info_data_7);
    BOOST_CHECK(history_info_data_10.at(4) == info_data_6);
    BOOST_CHECK(history_info_data_10.at(5) == info_data_5);
    BOOST_CHECK(history_info_data_10.at(6) == info_data_4);
    BOOST_CHECK(history_info_data_10.at(7) == info_data_3);
    BOOST_CHECK(history_info_data_10.at(8) == info_data_2);
    BOOST_CHECK(history_info_data_10.at(9) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_10.at(1).info_nsset_data == history_info_data_9.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_10.at(1).next_historyid.get_value(), history_info_data_10.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_10.at(0).info_nsset_data.crhistoryid, info_data_10.info_nsset_data.crhistoryid);

    //set logd request_id
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).set_logd_request_id(1).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_11 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_11 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_10_with_changes = info_data_10;

    //updated historyid
    BOOST_CHECK_NE(info_data_10.info_nsset_data.historyid, info_data_11.info_nsset_data.historyid);
    info_data_10_with_changes.info_nsset_data.historyid = info_data_11.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_10.info_nsset_data.history_uuid), get_raw_value_from(info_data_11.info_nsset_data.history_uuid));
    info_data_10_with_changes.info_nsset_data.history_uuid = info_data_11.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_11.info_nsset_data.update_registrar_handle.get_value());
    info_data_10_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_10_with_changes.info_nsset_data.update_time = info_data_11.info_nsset_data.update_time;

    //check changes made by last update
    BOOST_CHECK(info_data_10_with_changes == info_data_11);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_11.at(0) == info_data_11);
    BOOST_CHECK(history_info_data_11.at(1) == info_data_10);
    BOOST_CHECK(history_info_data_11.at(2) == info_data_9);
    BOOST_CHECK(history_info_data_11.at(3) == info_data_8);
    BOOST_CHECK(history_info_data_11.at(4) == info_data_7);
    BOOST_CHECK(history_info_data_11.at(5) == info_data_6);
    BOOST_CHECK(history_info_data_11.at(6) == info_data_5);
    BOOST_CHECK(history_info_data_11.at(7) == info_data_4);
    BOOST_CHECK(history_info_data_11.at(8) == info_data_3);
    BOOST_CHECK(history_info_data_11.at(9) == info_data_2);
    BOOST_CHECK(history_info_data_11.at(10) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_11.at(1).info_nsset_data == history_info_data_10.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_11.at(1).next_historyid.get_value(), history_info_data_11.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_11.at(0).info_nsset_data.crhistoryid, info_data_11.info_nsset_data.crhistoryid);

    //check logd request_id
    BOOST_CHECK_EQUAL(history_info_data_11.at(0).logd_request_id.get_value(), 1);

    //set tech check level
    ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle).set_tech_check_level(2).exec(ctx);

    const ::LibFred::InfoNssetOutput info_data_12 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    const std::vector<::LibFred::InfoNssetOutput> history_info_data_12 = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);

    ::LibFred::InfoNssetOutput info_data_11_with_changes = info_data_11;

    //updated historyid
    BOOST_CHECK_NE(info_data_11.info_nsset_data.historyid, info_data_12.info_nsset_data.historyid);
    info_data_11_with_changes.info_nsset_data.historyid = info_data_12.info_nsset_data.historyid;
    BOOST_CHECK_NE(get_raw_value_from(info_data_11.info_nsset_data.history_uuid), get_raw_value_from(info_data_12.info_nsset_data.history_uuid));
    info_data_11_with_changes.info_nsset_data.history_uuid = info_data_12.info_nsset_data.history_uuid;

    //updated update_registrar_handle
    BOOST_CHECK_EQUAL(registrar_handle, info_data_12.info_nsset_data.update_registrar_handle.get_value());
    info_data_11_with_changes.info_nsset_data.update_registrar_handle = registrar_handle;

    //updated update_time
    info_data_11_with_changes.info_nsset_data.update_time = info_data_12.info_nsset_data.update_time;

    //updated tech_check_level
    BOOST_CHECK_EQUAL(info_data_12.info_nsset_data.tech_check_level.get_value(), 2);
    info_data_11_with_changes.info_nsset_data.tech_check_level = 2;


    //check changes made by last update
    BOOST_CHECK(info_data_11_with_changes == info_data_12);

    //check info nsset history against info nsset
    BOOST_CHECK(history_info_data_12.at(0) == info_data_12);
    BOOST_CHECK(history_info_data_12.at(1) == info_data_11);
    BOOST_CHECK(history_info_data_12.at(2) == info_data_10);
    BOOST_CHECK(history_info_data_12.at(3) == info_data_9);
    BOOST_CHECK(history_info_data_12.at(4) == info_data_8);
    BOOST_CHECK(history_info_data_12.at(5) == info_data_7);
    BOOST_CHECK(history_info_data_12.at(6) == info_data_6);
    BOOST_CHECK(history_info_data_12.at(7) == info_data_5);
    BOOST_CHECK(history_info_data_12.at(8) == info_data_4);
    BOOST_CHECK(history_info_data_12.at(9) == info_data_3);
    BOOST_CHECK(history_info_data_12.at(10) == info_data_2);
    BOOST_CHECK(history_info_data_12.at(11) == info_data_1);

    //check info nsset history against last info nsset history
    BOOST_CHECK(history_info_data_12.at(1).info_nsset_data == history_info_data_11.at(0).info_nsset_data);

    //check historyid
    BOOST_CHECK_EQUAL(history_info_data_12.at(1).next_historyid.get_value(), history_info_data_12.at(0).info_nsset_data.historyid);
    BOOST_CHECK_EQUAL(history_info_data_12.at(0).info_nsset_data.crhistoryid, info_data_12.info_nsset_data.crhistoryid);

    ctx.commit_transaction();
}

/**
 * test UpdateNsset with wrong handle
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_wrong_handle, update_nsset_fixture )
{
    const std::string bad_test_nsset_handle = "bad" + test_nsset_handle;
    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(bad_test_nsset_handle, registrar_handle).exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_unknown_nsset_handle());
        BOOST_CHECK_EQUAL(static_cast<std::string>(e.get_unknown_nsset_handle()), bad_test_nsset_handle);
    }
}

/**
 * test UpdateNsset with wrong registrar
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_wrong_registrar, update_nsset_fixture)
{
    const std::string bad_registrar_handle = registrar_handle + xmark;
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, bad_registrar_handle).exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_unknown_registrar_handle());
        BOOST_CHECK_EQUAL(e.get_unknown_registrar_handle(), bad_registrar_handle);
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset add non-existing tech contact
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_add_wrong_tech_contact, update_nsset_fixture)
{
    const std::string bad_tech_contact_handle = admin_contact2_handle+xmark;
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .add_tech_contact(bad_tech_contact_handle)
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_vector_of_unknown_technical_contact_handle());
        BOOST_CHECK_EQUAL(e.get_vector_of_unknown_technical_contact_handle().at(0), bad_tech_contact_handle);
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset add already added tech contact
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_add_already_added_tech_contact, update_nsset_fixture)
{
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .add_tech_contact(admin_contact3_handle)//already added in fixture
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_vector_of_already_set_technical_contact_handle());
        BOOST_CHECK_EQUAL(e.get_vector_of_already_set_technical_contact_handle().at(0), admin_contact3_handle);
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset remove non-existing tech contact
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_rem_wrong_tech_contact, update_nsset_fixture)
{
    const std::string bad_tech_contact_handle = admin_contact2_handle + xmark;
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .rem_tech_contact(bad_tech_contact_handle)
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_vector_of_unknown_technical_contact_handle());
        BOOST_CHECK_EQUAL(e.get_vector_of_unknown_technical_contact_handle().at(0), bad_tech_contact_handle);
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset remove existing unassigned tech contact
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_rem_unassigned_tech_contact, update_nsset_fixture)
{
    const std::string bad_tech_contact_handle = admin_contact2_handle;
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .rem_tech_contact(bad_tech_contact_handle)
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& ex)
    {
        BOOST_CHECK(ex.is_set_vector_of_unassigned_technical_contact_handle());
        BOOST_CHECK(ex.get_vector_of_unassigned_technical_contact_handle().at(0).compare(bad_tech_contact_handle) == 0);
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset add already added dnshost
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_add_already_added_dnshost, update_nsset_fixture)
{
    namespace ip = boost::asio::ip;

    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .add_dns(::LibFred::DnsHost("a.ns.nic.cz", { ip::address::from_string("127.0.0.3"), ip::address::from_string("127.1.1.3") }))//already added in fixture
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_vector_of_already_set_dns_host());
        BOOST_CHECK_EQUAL(e.get_vector_of_already_set_dns_host().at(0), "a.ns.nic.cz");
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test UpdateNsset remove unassigned dnshost
 */
BOOST_FIXTURE_TEST_CASE(update_nsset_remove_unassigned_dnshost, update_nsset_fixture)
{
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }

    try
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .rem_dns("c.ns.nic.cz")
        .exec(ctx);
        ctx.commit_transaction();
        BOOST_ERROR("no exception thrown");
    }
    catch (const ::LibFred::UpdateNsset::Exception& e)
    {
        BOOST_CHECK(e.is_set_vector_of_unassigned_dns_host());
        BOOST_CHECK_EQUAL(e.get_vector_of_unassigned_dns_host().at(0), "c.ns.nic.cz");
    }

    ::LibFred::InfoNssetOutput info_data_2;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    BOOST_CHECK(info_data_1 == info_data_2);
    BOOST_CHECK(info_data_2.info_nsset_data.delete_time.isnull());
}

/**
 * test InfoNssetHistoryByRoid
 * create and update test nsset
 * compare successive states from info nsset with states from info nsset history
 * check initial and next historyid in info nsset history
 * check valid_from and valid_to in info nsset history
 */
BOOST_FIXTURE_TEST_CASE(info_nsset_history_test, update_nsset_fixture)
{
    ::LibFred::InfoNssetOutput info_data_1;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_1 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
    }
    //call update
    {
        ::LibFred::OperationContextCreator ctx;//new connection to rollback on error
        ::LibFred::UpdateNsset(test_nsset_handle, registrar_handle)
        .exec(ctx);
        ctx.commit_transaction();
    }

    ::LibFred::InfoNssetOutput info_data_2;
    std::vector<::LibFred::InfoNssetOutput> history_info_data;
    {
        ::LibFred::OperationContextCreator ctx;
        info_data_2 = ::LibFred::InfoNssetByHandle(test_nsset_handle).exec(ctx);
        history_info_data = ::LibFred::InfoNssetHistoryByRoid(info_data_1.info_nsset_data.roid).exec(ctx);
    }

    BOOST_CHECK(history_info_data.at(0) == info_data_2);
    BOOST_CHECK(history_info_data.at(1) == info_data_1);

    BOOST_CHECK_EQUAL(history_info_data.at(1).next_historyid.get_value(), history_info_data.at(0).info_nsset_data.historyid);

    BOOST_CHECK(history_info_data.at(1).history_valid_from < history_info_data.at(1).history_valid_to.get_value());
    BOOST_CHECK(history_info_data.at(1).history_valid_to.get_value() <= history_info_data.at(0).history_valid_from);
    BOOST_CHECK(history_info_data.at(0).history_valid_to.isnull());

    BOOST_CHECK_EQUAL(history_info_data.at(1).info_nsset_data.crhistoryid, history_info_data.at(1).info_nsset_data.historyid);
}

BOOST_AUTO_TEST_SUITE_END()//TestUpdateNsset
