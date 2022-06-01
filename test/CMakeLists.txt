find_package(Boost 1.53.0
    COMPONENTS
    unit_test_framework
    REQUIRED)

add_library(fred-test-setup STATIC
    test/setup/arguments.cc
    test/setup/cfg.cc
    test/setup/fixtures.cc
    test/setup/fixtures_utils.cc
    test/setup/run_in_background.cc
    test/setup/test_tree_list.cc)
add_library(FredTestSetup::library ALIAS fred-test-setup)

target_include_directories(fred-test-setup
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${PostgreSQL_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(fred-test-setup PUBLIC
    Boost::unit_test_framework
    Boost::program_options
    Fred::library
    ${PostgreSQL_LIBRARIES})

set_target_properties(fred-test-setup PROPERTIES
    VERSION ${PROJECT_VERSION}
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib"
    OUTPUT_NAME "fred-test-setup")

add_library(fred-test-common-static STATIC
    test/libfred/contact/verification/setup_utils.cc
    test/libfred/registrar/epp_auth/util.cc)
add_library(fred-test-common-static::library ALIAS fred-test-common-static)

target_include_directories(fred-test-common-static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${PostgreSQL_INCLUDE_DIRS})

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
    test/util/test_db.cc)

get_directory_property(LIBFRED_HAS_PARENT PARENT_DIRECTORY)
if(LIBFRED_HAS_PARENT)
    set(${PROJECT_NAME}_INCLUDE_DIRS ${LIBFRED_INCLUDE_DIRS} PARENT_SCOPE)
    set(${PROJECT_NAME}_LIBRARIES ${LIBFRED_LIBRARIES} PARENT_SCOPE)
    set(LIBFRED_VERSION ${LIBFRED_VERSION} PARENT_SCOPE)
endif()

set_target_properties(test-libfred PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

target_include_directories(test-libfred PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/..
    ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(test-libfred PRIVATE
    Fred::library
    fred-test-common-static::library
    FredTestSetup::library
    Boost::unit_test_framework
    Boost::program_options
    ${LIBFRED_LIBRARIES})
include(GNUInstallDirs)

install(TARGETS test-libfred DESTINATION ${CMAKE_INSTALL_BINDIR})
