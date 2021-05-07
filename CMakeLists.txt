cmake_minimum_required(VERSION 3.5)

set(LIBFRED_VERSION "4.3.0")
project(LibFred
    VERSION ${LIBFRED_VERSION}
    LANGUAGES CXX)

include(GNUInstallDirs)

set(LIBFRED_SOURCES
    src/libfred/registrable_object/contact/verification/cancel_states.cc
    src/libfred/opcontext.cc
    src/libfred/notifier/enqueue_notification.cc
    src/libfred/notifier/process_one_notification_request.cc
    src/libfred/notifier/gather_email_data/gather_email_addresses.cc
    src/libfred/notifier/gather_email_data/gather_email_content.cc
    src/libfred/notifier/gather_email_data/objecttype_specific_impl/contact.cc
    src/libfred/notifier/gather_email_data/objecttype_specific_impl/domain.cc
    src/libfred/notifier/gather_email_data/objecttype_specific_impl/keyset.cc
    src/libfred/notifier/gather_email_data/objecttype_specific_impl/nsset.cc
    src/libfred/object/check_handle.cc
    src/libfred/object/generate_authinfo_password.cc
    src/libfred/object/generated_authinfo_password.cc
    src/libfred/object/get_id_of_registered.cc
    src/libfred/object/object.cc
    src/libfred/object/object_id_handle_pair.cc
    src/libfred/object/object_impl.cc
    src/libfred/object/transfer_object.cc
    src/libfred/object_state/cancel_object_state_request_id.cc
    src/libfred/object_state/clear_admin_object_state_request_id.cc
    src/libfred/object_state/clear_object_state_request_id.cc
    src/libfred/object_state/create_admin_object_block_request_id.cc
    src/libfred/object_state/create_admin_object_state_restore_request_id.cc
    src/libfred/object_state/create_object_state_request_id.cc
    src/libfred/object_state/get_blocking_status_desc_list.cc
    src/libfred/object_state/get_object_state_descriptions.cc
    src/libfred/object_state/get_object_state_id_map.cc
    src/libfred/object_state/get_object_states.cc
    src/libfred/object_state/get_object_states_by_history_id.cc
    src/libfred/object_state/lock_object_state_request_lock.cc
    src/libfred/object_state/object_has_state.cc
    src/libfred/object_state/perform_object_state_request.cc
    src/libfred/poll/create_low_credit_messages.cc
    src/libfred/poll/create_poll_message.cc
    src/libfred/poll/create_request_fee_info_message.cc
    src/libfred/poll/create_state_messages.cc
    src/libfred/poll/create_update_object_poll_message.cc
    src/libfred/poll/get_request_fee_message.cc
    src/libfred/public_request/create_public_request.cc
    src/libfred/public_request/create_public_request_auth.cc
    src/libfred/public_request/get_opened_public_request.cc
    src/libfred/public_request/info_public_request.cc
    src/libfred/public_request/info_public_request_auth.cc
    src/libfred/public_request/public_request_lock_guard.cc
    src/libfred/public_request/public_request_object_lock_guard.cc
    src/libfred/public_request/update_public_request.cc
    src/libfred/registrable_object/history_interval.cc
    src/libfred/registrable_object/registrable_object_reference.cc
    src/libfred/registrable_object/uuid.cc
    src/libfred/registrable_object/contact/check_contact.cc
    src/libfred/registrable_object/contact/contact_data_history.cc
    src/libfred/registrable_object/contact/contact_handle_history.cc
    src/libfred/registrable_object/contact/contact_state.cc
    src/libfred/registrable_object/contact/copy_contact.cc
    src/libfred/registrable_object/contact/copy_history_impl.cc
    src/libfred/registrable_object/contact/create_contact.cc
    src/libfred/registrable_object/contact/delete_contact.cc
    src/libfred/registrable_object/contact/find_contact_duplicates.cc
    src/libfred/registrable_object/contact/get_contact_data_history.cc
    src/libfred/registrable_object/contact/get_contact_handle_history.cc
    src/libfred/registrable_object/contact/get_contact_state.cc
    src/libfred/registrable_object/contact/get_contact_state_history.cc
    src/libfred/registrable_object/contact/info_contact.cc
    src/libfred/registrable_object/contact/info_contact_data.cc
    src/libfred/registrable_object/contact/info_contact_diff.cc
    src/libfred/registrable_object/contact/info_contact_impl.cc
    src/libfred/registrable_object/contact/info_contact_output.cc
    src/libfred/registrable_object/contact/merge_contact.cc
    src/libfred/registrable_object/contact/merge_contact_email_notification_data.cc
    src/libfred/registrable_object/contact/merge_contact_selection.cc
    src/libfred/registrable_object/contact/place_address.cc
    src/libfred/registrable_object/contact/transfer_contact.cc
    src/libfred/registrable_object/contact/undisclose_address.cc
    src/libfred/registrable_object/contact/update_contact.cc
    src/libfred/registrable_object/contact/verification/create_check.cc
    src/libfred/registrable_object/contact/verification/create_test.cc
    src/libfred/registrable_object/contact/verification/info_check.cc
    src/libfred/registrable_object/contact/verification/list_checks.cc
    src/libfred/registrable_object/contact/verification/list_enum_objects.cc
    src/libfred/registrable_object/contact/verification/update_check.cc
    src/libfred/registrable_object/contact/verification/update_test.cc
    src/libfred/registrable_object/domain/check_domain.cc
    src/libfred/registrable_object/domain/copy_history_impl.cc
    src/libfred/registrable_object/domain/create_domain.cc
    src/libfred/registrable_object/domain/create_domain_name_blacklist_id.cc
    src/libfred/registrable_object/domain/delete_domain.cc
    src/libfred/registrable_object/domain/domain.cc
    src/libfred/registrable_object/domain/domain_data_history.cc
    src/libfred/registrable_object/domain/domain_name.cc
    src/libfred/registrable_object/domain/domain_state.cc
    src/libfred/registrable_object/domain/fqdn_history.cc
    src/libfred/registrable_object/domain/get_domain_data_history.cc
    src/libfred/registrable_object/domain/get_domain_state.cc
    src/libfred/registrable_object/domain/get_domain_state_history.cc
    src/libfred/registrable_object/domain/get_fqdn_history.cc
    src/libfred/registrable_object/domain/info_domain.cc
    src/libfred/registrable_object/domain/info_domain_data.cc
    src/libfred/registrable_object/domain/info_domain_diff.cc
    src/libfred/registrable_object/domain/info_domain_impl.cc
    src/libfred/registrable_object/domain/info_domain_output.cc
    src/libfred/registrable_object/domain/renew_domain.cc
    src/libfred/registrable_object/domain/transfer_domain.cc
    src/libfred/registrable_object/domain/update_domain.cc
    src/libfred/registrable_object/keyset/get_keyset_data_history.cc
    src/libfred/registrable_object/keyset/get_keyset_handle_history.cc
    src/libfred/registrable_object/keyset/get_keyset_state.cc
    src/libfred/registrable_object/keyset/get_keyset_state_history.cc
    src/libfred/registrable_object/keyset/check_dns_key.cc
    src/libfred/registrable_object/keyset/check_keyset.cc
    src/libfred/registrable_object/keyset/copy_history_impl.cc
    src/libfred/registrable_object/keyset/create_keyset.cc
    src/libfred/registrable_object/keyset/delete_keyset.cc
    src/libfred/registrable_object/keyset/info_keyset.cc
    src/libfred/registrable_object/keyset/info_keyset_data.cc
    src/libfred/registrable_object/keyset/info_keyset_diff.cc
    src/libfred/registrable_object/keyset/info_keyset_impl.cc
    src/libfred/registrable_object/keyset/info_keyset_output.cc
    src/libfred/registrable_object/keyset/keyset_data_history.cc
    src/libfred/registrable_object/keyset/keyset_handle_history.cc
    src/libfred/registrable_object/keyset/keyset_state.cc
    src/libfred/registrable_object/keyset/transfer_keyset.cc
    src/libfred/registrable_object/keyset/update_keyset.cc
    src/libfred/registrable_object/nsset/check_nsset.cc
    src/libfred/registrable_object/nsset/copy_history_impl.cc
    src/libfred/registrable_object/nsset/create_nsset.cc
    src/libfred/registrable_object/nsset/delete_nsset.cc
    src/libfred/registrable_object/nsset/get_nsset_data_history.cc
    src/libfred/registrable_object/nsset/get_nsset_handle_history.cc
    src/libfred/registrable_object/nsset/get_nsset_state.cc
    src/libfred/registrable_object/nsset/get_nsset_state_history.cc
    src/libfred/registrable_object/nsset/info_nsset.cc
    src/libfred/registrable_object/nsset/info_nsset_data.cc
    src/libfred/registrable_object/nsset/info_nsset_diff.cc
    src/libfred/registrable_object/nsset/info_nsset_impl.cc
    src/libfred/registrable_object/nsset/info_nsset_output.cc
    src/libfred/registrable_object/nsset/nsset_data_history.cc
    src/libfred/registrable_object/nsset/nsset_handle_history.cc
    src/libfred/registrable_object/nsset/nsset_state.cc
    src/libfred/registrable_object/nsset/transfer_nsset.cc
    src/libfred/registrable_object/nsset/update_nsset.cc
    src/libfred/registrar/check_registrar.cc
    src/libfred/registrar/create_registrar.cc
    src/libfred/registrar/exceptions.cc
    src/libfred/registrar/get_registrar_handles.cc
    src/libfred/registrar/get_registrar_zone_credit.cc
    src/libfred/registrar/info_registrar.cc
    src/libfred/registrar/info_registrar_data.cc
    src/libfred/registrar/info_registrar_diff.cc
    src/libfred/registrar/info_registrar_impl.cc
    src/libfred/registrar/info_registrar_output.cc
    src/libfred/registrar/registrar_zone_access.cc
    src/libfred/registrar/registrar_zone_credit.cc
    src/libfred/registrar/update_registrar.cc
    src/libfred/registrar/zone_credit.cc
    src/libfred/registrar/certification/create_registrar_certification.cc
    src/libfred/registrar/certification/get_registrar_certifications.cc
    src/libfred/registrar/certification/update_registrar_certification.cc
    src/libfred/registrar/credit/create_registrar_credit_transaction.cc
    src/libfred/registrar/credit/exceptions.cc
    src/libfred/registrar/epp_auth/add_registrar_epp_auth.cc
    src/libfred/registrar/epp_auth/clone_registrar_epp_auth.cc
    src/libfred/registrar/epp_auth/delete_registrar_epp_auth.cc
    src/libfred/registrar/epp_auth/exceptions.cc
    src/libfred/registrar/epp_auth/get_registrar_epp_auth.cc
    src/libfred/registrar/epp_auth/update_registrar_epp_auth.cc
    src/libfred/registrar/group/cancel_registrar_group.cc
    src/libfred/registrar/group/create_registrar_group.cc
    src/libfred/registrar/group/get_registrar_groups.cc
    src/libfred/registrar/group/update_registrar_group.cc
    src/libfred/registrar/group/membership/create_registrar_group_membership.cc
    src/libfred/registrar/group/membership/end_registrar_group_membership.cc
    src/libfred/registrar/group/membership/info_group_membership_by_group.cc
    src/libfred/registrar/group/membership/info_group_membership_by_registrar.cc
    src/libfred/registrar/zone_access/add_registrar_zone_access.cc
    src/libfred/registrar/zone_access/exceptions.cc
    src/libfred/registrar/zone_access/get_zone_access_history.cc
    src/libfred/registrar/zone_access/registrar_zone_access_history.cc
    src/libfred/registrar/zone_access/update_registrar_zone_access.cc
    src/libfred/zone/create_zone.cc
    src/libfred/zone/exceptions.cc
    src/libfred/zone/info_zone.cc
    src/libfred/zone/info_zone_data.cc
    src/libfred/zone/update_zone.cc
    src/libfred/zone/util.cc
    src/libfred/zone/zone.cc
    src/libfred/zone/zone_ns/create_zone_ns.cc
    src/libfred/zone/zone_ns/exceptions.cc
    src/libfred/zone/zone_ns/info_zone_ns.cc
    src/libfred/zone/zone_ns/update_zone_ns.cc
    src/libfred/zone/zone_ns/util.cc
    src/libfred/zone/zone_soa/create_zone_soa.cc
    src/libfred/zone/zone_soa/info_zone_soa.cc
    src/libfred/zone/zone_soa/update_zone_soa.cc
    src/util/case_insensitive.cc
    src/util/idn_utils.cc
    src/util/password_storage.cc
    src/util/printable.cc
    src/util/util.cc
    src/util/db/param_query_composition.cc
    src/util/db/value.cc
    src/util/db/psql/psql_connection.cc
    src/util/db/psql/psql_result.cc
    src/util/log/context.cc
    src/util/log/log.cc
    src/util/log/log_device.cc
    src/util/password_storage/base64.cc
    src/util/password_storage/impl/check_equality.cc
    src/util/password_storage/impl/pbkdf2.cc
    src/util/password_storage/impl/plaintext.cc
    src/util/types/date.cc
    src/util/types/date_interval.cc
    src/util/types/datetime.cc
    src/util/types/datetime_interval.cc
    src/util/types/id.cc
    src/util/random/algorithm/boost_date.cc
    src/util/random/algorithm/boost_time.cc
    src/util/random/seeders/nondeterministic_seeder.cc)

add_library(fred STATIC ${LIBFRED_SOURCES})
add_library(Fred::library ALIAS fred)

set_target_properties(fred PROPERTIES
    VERSION ${PROJECT_VERSION}
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    OUTPUT_NAME "fred")

target_compile_options(fred
    PUBLIC "-ggdb"
    PUBLIC "-grecord-gcc-switches"
    PUBLIC "-Wall"
    PUBLIC "-Wextra"
    PUBLIC "-fdiagnostics-color=auto")

target_include_directories(fred PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

set(PACKAGE libfred)
set(HAVE_BOOST_SERIALIZATION 1)
set(HAVE_LOGGER 1)

set(VERSION ${PROJECT_VERSION})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(fred.pc.in fred.pc @ONLY)

include(CheckIncludeFileCXX)
check_include_file_cxx("mpdecimal.h" FOUND_mpdecimal_h)
if(NOT FOUND_mpdecimal_h)
    message(FATAL_ERROR "Required header mpdecimal.h not found!")
endif()
set(Mpdec_LIBRARY mpdec)

find_package(Threads REQUIRED)
set(Threads_LIBRARIES Threads::Threads)
set(PostgreSQL_ADDITIONAL_VERSIONS "9.6")
find_package(PostgreSQL ${PostgreSQL_ADDITIONAL_VERSIONS} REQUIRED)
find_package(Boost 1.53.0
    COMPONENTS
    system
    thread
    date_time
    program_options
    regex
    random
    serialization
    filesystem
    unit_test_framework
    REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(LIBIDN REQUIRED libidn>=0.5.18)
pkg_search_module(LIBCRYPTO REQUIRED libcrypto)

set(LIBFRED_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PostgreSQL_INCLUDE_DIRS})

target_include_directories(fred PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBFRED_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR})

set(LIBFRED_LIBRARIES
    Threads::Threads
    Boost::system
    Boost::thread
    Boost::date_time
    Boost::random
    Boost::regex
    ${Mpdec_LIBRARY}
    ${LIBIDN_LIBRARIES}
    ${LIBCRYPTO_LIBRARIES}
    ${PostgreSQL_LIBRARIES})

target_link_libraries(fred
    ${LIBFRED_LIBRARIES})

add_library(fred-test-common-static STATIC
    test/libfred/contact/verification/setup_utils.cc
    test/libfred/registrar/epp_auth/util.cc)
add_library(fred-test-common-static::library ALIAS fred-test-common-static)

target_include_directories(fred-test-common-static
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(fred-test-common-static PUBLIC
    Boost::unit_test_framework
    Boost::program_options
    Fred::library)

set_target_properties(fred-test-common-static PROPERTIES
    VERSION ${PROJECT_VERSION}
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    OUTPUT_NAME "fred-test-common")

add_executable(test-libfred
    test/fake-src/util/tz/europe/prague.cc
    test/libfred/main.cc
    test/libfred/test_check_handle.cc
    test/libfred/test_flagset.cc
    test/libfred/test_opexception.cc
#    test/libfred/contact/test_contact_history.cc
#    test/libfred/contact/test_contact_state.cc
    test/libfred/contact/test_copy_contact.cc
    test/libfred/contact/test_create_contact.cc
    test/libfred/contact/test_delete_contact.cc
    test/libfred/contact/test_info_contact.cc
    test/libfred/contact/test_merge_contact.cc
    test/libfred/contact/test_transfer_contact.cc
    test/libfred/contact/test_update_contact.cc
    test/libfred/contact/test_undisclose_address.cc
    test/libfred/contact/verification/test_create_check.cc
    test/libfred/contact/verification/test_create_test.cc
    test/libfred/contact/verification/test_django_email_format_check.cc
    test/libfred/contact/verification/test_info_check.cc
    test/libfred/contact/verification/test_list_enum_objects.cc
    test/libfred/contact/verification/test_list_checks.cc
    test/libfred/contact/verification/test_update_check.cc
    test/libfred/contact/verification/test_update_test.cc
    test/libfred/contact/verification/util.cc
    test/libfred/domain/test_create_domain.cc
    test/libfred/domain/test_create_domain_name_blacklist_id.cc
    test/libfred/domain/test_delete_domain.cc
    test/libfred/domain/test_domain_name.cc
    test/libfred/domain/test_check_domain.cc
    test/libfred/domain/test_info_domain.cc
    test/libfred/domain/test_renew_domain.cc
    test/libfred/domain/test_transfer_domain.cc
    test/libfred/domain/test_update_domain.cc
    test/libfred/keyset/test_delete_keyset.cc
    test/libfred/keyset/test_info_keyset.cc
    test/libfred/keyset/test_transfer_keyset.cc
    test/libfred/keyset/test_update_keyset.cc
    test/libfred/notifier/test_enqueue_notification.cc
    test/libfred/notifier/test_event_on_object_enum.cc
    test/libfred/notifier/test_gather_email_addresses_contact_create.cc
    test/libfred/notifier/test_gather_email_addresses_contact_delete.cc
    test/libfred/notifier/test_gather_email_addresses_contact_transfer.cc
    test/libfred/notifier/test_gather_email_addresses_contact_update.cc
    test/libfred/notifier/test_gather_email_addresses_domain_create.cc
    test/libfred/notifier/test_gather_email_addresses_domain_delete.cc
    test/libfred/notifier/test_gather_email_addresses_domain_renew.cc
    test/libfred/notifier/test_gather_email_addresses_domain_transfer.cc
    test/libfred/notifier/test_gather_email_addresses_domain_update.cc
    test/libfred/notifier/test_gather_email_addresses_keyset_create.cc
    test/libfred/notifier/test_gather_email_addresses_keyset_delete.cc
    test/libfred/notifier/test_gather_email_addresses_keyset_transfer.cc
    test/libfred/notifier/test_gather_email_addresses_keyset_update.cc
    test/libfred/notifier/test_gather_email_addresses_nsset_create.cc
    test/libfred/notifier/test_gather_email_addresses_nsset_delete.cc
    test/libfred/notifier/test_gather_email_addresses_nsset_transfer.cc
    test/libfred/notifier/test_gather_email_addresses_nsset_update.cc
    test/libfred/notifier/test_gather_email_content_contact_create.cc
    test/libfred/notifier/test_gather_email_content_contact_delete.cc
    test/libfred/notifier/test_gather_email_content_contact_transfer.cc
    test/libfred/notifier/test_gather_email_content_contact_update.cc
    test/libfred/notifier/test_gather_email_content_domain_create.cc
    test/libfred/notifier/test_gather_email_content_domain_delete.cc
    test/libfred/notifier/test_gather_email_content_domain_renew.cc
    test/libfred/notifier/test_gather_email_content_domain_transfer.cc
    test/libfred/notifier/test_gather_email_content_domain_update.cc
    test/libfred/notifier/test_gather_email_content_keyset_create.cc
    test/libfred/notifier/test_gather_email_content_keyset_delete.cc
    test/libfred/notifier/test_gather_email_content_keyset_transfer.cc
    test/libfred/notifier/test_gather_email_content_keyset_update.cc
    test/libfred/notifier/test_gather_email_content_nsset_create.cc
    test/libfred/notifier/test_gather_email_content_nsset_delete.cc
    test/libfred/notifier/test_gather_email_content_nsset_transfer.cc
    test/libfred/notifier/test_gather_email_content_nsset_update.cc
    test/libfred/notifier/test_process_one_notification_request.cc
    test/libfred/object/test_generate_authinfo_password.cc
    test/libfred/object/test_generated_authinfo_password.cc
    test/libfred/object/test_registry_object_type.cc
    test/libfred/object/test_transfer_object.cc
    test/libfred/object_state/test_cancel_object_state_request_id.cc
    test/libfred/object_state/test_create_admin_object_block_request_id.cc
    test/libfred/object_state/test_create_admin_object_state_restore_request_id.cc
    test/libfred/object_state/test_create_object_state_request_id.cc
    test/libfred/object_state/test_object_state.cc
    test/libfred/poll/test_create_delete_contact_poll_message.cc
    test/libfred/poll/test_create_delete_domain_poll_message.cc
    test/libfred/poll/test_create_state_messages.cc
    test/libfred/poll/test_create_update_object_poll_message.cc
    test/libfred/poll/test_get_request_fee_info_message.cc
    test/libfred/public_request/test_create_public_request.cc
    test/libfred/public_request/test_create_public_request_auth.cc
    test/libfred/public_request/test_update_public_request.cc
    test/libfred/nsset/test_delete_nsset.cc
    test/libfred/nsset/test_info_nsset.cc
    test/libfred/nsset/test_transfer_nsset.cc
    test/libfred/nsset/test_update_nsset.cc
    test/libfred/random/mock_engine.cc
    test/libfred/random/mock_seeder.cc
    test/libfred/random/test_random.cc
    test/libfred/registrar/test_create_registrar.cc
    test/libfred/registrar/test_update_registrar.cc
    test/libfred/registrar/test_get_registrar_zone_credit.cc
    test/libfred/registrar/test_info_registrar.cc
    test/libfred/registrar/test_registrar_zone_access.cc
    test/libfred/registrar/certification/test_create_registrar_certification.cc
    test/libfred/registrar/certification/test_get_registrar_certifications.cc
    test/libfred/registrar/certification/test_update_registrar_certification.cc
    test/libfred/registrar/credit/test_create_registrar_credit_transaction.cc
    test/libfred/registrar/group/test_cancel.cc
    test/libfred/registrar/group/test_create.cc
    test/libfred/registrar/group/test_get.cc
    test/libfred/registrar/group/test_update.cc
    test/libfred/registrar/group/membership/test_create.cc
    test/libfred/registrar/group/membership/test_end.cc
    test/libfred/registrar/group/membership/test_info_by_group.cc
    test/libfred/registrar/group/membership/test_info_by_registrar.cc
    test/libfred/registrar/epp_auth/test_add_registrar_epp_auth.cc
    test/libfred/registrar/epp_auth/test_get_registrar_epp_auth.cc
    test/libfred/registrar/epp_auth/test_clone_registrar_epp_auth.cc
    test/libfred/registrar/epp_auth/test_update_registrar_epp_auth.cc
    test/libfred/registrar/epp_auth/test_delete_registrar_epp_auth.cc
    test/libfred/registrar/zone_access/test_add_zone_access.cc
    test/libfred/registrar/zone_access/test_update_zone_access.cc
    test/libfred/registrar/zone_access/util.cc
    test/libfred/zone/test_create_zone.cc
    test/libfred/zone/test_info_zone.cc
    test/libfred/zone/test_update_zone.cc
    test/libfred/zone/util.cc
    test/libfred/zone/zone_ns/test_create_zone_ns.cc
    test/libfred/zone/zone_ns/test_info_zone_ns.cc
    test/libfred/zone/zone_ns/test_update_zone_ns.cc
    test/libfred/zone/zone_ns/util.cc
    test/libfred/zone/zone_soa/test_create_zone_soa.cc
    test/libfred/zone/zone_soa/test_info_zone_soa.cc
    test/libfred/zone/zone_soa/test_update_zone_soa.cc
    test/libfred/zone/zone_soa/util.cc
    test/util/test_case_insensitive.cc
    test/util/test_password_storage.cc
    test/util/test_db.cc
    test/setup/fixtures.cc
    test/setup/fixtures_utils.cc)

set_target_properties(test-libfred PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

target_include_directories(test-libfred PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIBFRED_INCLUDE_DIRS})

target_link_libraries(test-libfred PRIVATE
    Fred::library
    fred-test-common-static::library
    Boost::unit_test_framework
    Boost::program_options
    ${LIBFRED_LIBRARIES})

get_directory_property(LIBFRED_HAS_PARENT PARENT_DIRECTORY)
if(LIBFRED_HAS_PARENT)
    set(${PROJECT_NAME}_INCLUDE_DIRS ${LIBFRED_INCLUDE_DIRS} PARENT_SCOPE)
    set(${PROJECT_NAME}_LIBRARIES ${LIBFRED_LIBRARIES} PARENT_SCOPE)
    set(LIBFRED_VERSION ${LIBFRED_VERSION} PARENT_SCOPE)
endif()

find_program(GIT_PROGRAM git)
if("${LIBFRED_HAS_PARENT}" STREQUAL "")
    include(GNUInstallDirs)

    install(TARGETS fred
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fred.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

    add_custom_target(uninstall
        COMMAND xargs -L10 rm -v < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
        COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt | xargs -L1 dirname | sort | uniq | xargs -L10 rmdir -p --ignore-fail-on-non-empty)

    if(EXISTS ${CMAKE_SOURCE_DIR}/.git AND GIT_PROGRAM)
        string(TOLOWER "${PROJECT_NAME}-${PROJECT_VERSION}" TARBALL_FILE_NAME)
        if(NOT TARGET dist)
            add_custom_command(OUTPUT ${TARBALL_FILE_NAME}.tar.gz
                COMMAND ${GIT_PROGRAM} archive --format=tar.gz --prefix=${TARBALL_FILE_NAME}/ HEAD -o ${CMAKE_BINARY_DIR}/${TARBALL_FILE_NAME}.tar.gz
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
            add_custom_target(dist DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${TARBALL_FILE_NAME}.tar.gz)
        endif()

        if(DEFINED ENV{DISTCHECK_CMAKE_FLAGS})
            set(distcheck_flags $ENV{DISTCHECK_CMAKE_FLAGS})
        endif()

        if(NOT TARGET distcheck)
            set(DISTCHECK_BUILD_DIR "_build")
            set(DISTCHECK_INSTALL_DIR "_inst")
            if(${CMAKE_GENERATOR} STREQUAL "Ninja")
                set(DISTCHECK_BUILD_CMD "ninja")
            else()
                set(DISTCHECK_BUILD_CMD "$(MAKE)")
            endif()
            add_custom_target(clean_before_distcheck
                COMMAND rm -rf ${DISTCHECK_BUILD_DIR} ${DISTCHECK_INSTALL_DIR} ${TARBALL_FILE_NAME}
                COMMENT "cleaning before distcheck start")
            add_custom_target(distcheck_make_dirs
                COMMAND mkdir ${DISTCHECK_BUILD_DIR} ${DISTCHECK_INSTALL_DIR}
                COMMENT "make distcheck build and install directories"
                DEPENDS clean_before_distcheck)
            add_custom_target(distcheck_unpack_archive
                COMMAND tar xzf ${TARBALL_FILE_NAME}.tar.gz
                COMMENT "Checking the distribution archive..."
                DEPENDS dist distcheck_make_dirs)
            add_custom_target(distcheck_configure
                COMMAND ${CMAKE_COMMAND} -G${CMAKE_GENERATOR} ${distcheck_flags} -DCMAKE_INSTALL_PREFIX=../${DISTCHECK_INSTALL_DIR} ../${TARBALL_FILE_NAME}
                COMMENT "configure distcheck"
                DEPENDS distcheck_unpack_archive
                WORKING_DIRECTORY ${DISTCHECK_BUILD_DIR})
            add_custom_target(distcheck_make
                COMMAND ${DISTCHECK_BUILD_CMD}
                COMMENT "make"
                DEPENDS distcheck_configure
                WORKING_DIRECTORY ${DISTCHECK_BUILD_DIR})
            add_custom_target(distcheck_install
                COMMAND ${DISTCHECK_BUILD_CMD} install
                COMMENT "make install"
                DEPENDS distcheck_make
                WORKING_DIRECTORY ${DISTCHECK_BUILD_DIR})
            add_custom_target(distcheck_uninstall
                COMMAND ${DISTCHECK_BUILD_CMD} uninstall
                COMMENT "make uninstall"
                DEPENDS distcheck_install
                WORKING_DIRECTORY ${DISTCHECK_BUILD_DIR})
            add_custom_target(distcheck_clean
                COMMAND ${DISTCHECK_BUILD_CMD} clean
                COMMENT "make clean"
                DEPENDS distcheck_uninstall
                WORKING_DIRECTORY ${DISTCHECK_BUILD_DIR})
            add_custom_target(distcheck
                COMMAND rm -rf ${DISTCHECK_BUILD_DIR} ${DISTCHECK_INSTALL_DIR} ${TARBALL_FILE_NAME}
                COMMENT "PASS: '${TARBALL_FILE_NAME}.tar.gz' is ready for distribution."
                DEPENDS distcheck_clean)
        endif()
    else()
        message(STATUS "no git -- not including dist and distcheck targets")
    endif()
elseif(EXISTS ${CMAKE_SOURCE_DIR}/.git AND GIT_PROGRAM)
    if(NOT TARGET dist_libfred)
        message(STATUS "LIBFRED_PREFIX: ${LIBFRED_PREFIX}")
        add_custom_target(dist_libfred
            COMMAND ${GIT_PROGRAM} archive --format=tar --prefix=${LIBFRED_PREFIX}/ HEAD > ${CMAKE_BINARY_DIR}/.libfred.tar
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endif()
