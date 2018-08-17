cmake_minimum_required(VERSION 3.5)

project(fred
    VERSION 0.1.0
    LANGUAGES CXX)

if("CXX" IN_LIST languages)
    message("C++ enabled")
endif()

include(GNUInstallDirs)

add_library(fred SHARED
    src/backend/admin/contact/verification/contact_states/delete_all.cc
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
    src/libfred/registrable_object/contact/check_contact.cc
    src/libfred/registrable_object/contact/copy_contact.cc
    src/libfred/registrable_object/contact/copy_history_impl.cc
    src/libfred/registrable_object/contact/create_contact.cc
    src/libfred/registrable_object/contact/delete_contact.cc
    src/libfred/registrable_object/contact/find_contact_duplicates.cc
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
    src/libfred/registrable_object/domain/domain_name.cc
    src/libfred/registrable_object/domain/info_domain.cc
    src/libfred/registrable_object/domain/info_domain_data.cc
    src/libfred/registrable_object/domain/info_domain_diff.cc
    src/libfred/registrable_object/domain/info_domain_impl.cc
    src/libfred/registrable_object/domain/info_domain_output.cc
    src/libfred/registrable_object/domain/renew_domain.cc
    src/libfred/registrable_object/domain/transfer_domain.cc
    src/libfred/registrable_object/domain/update_domain.cc
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
    src/libfred/registrable_object/keyset/transfer_keyset.cc
    src/libfred/registrable_object/keyset/update_keyset.cc
    src/libfred/registrable_object/nsset/check_nsset.cc
    src/libfred/registrable_object/nsset/copy_history_impl.cc
    src/libfred/registrable_object/nsset/create_nsset.cc
    src/libfred/registrable_object/nsset/delete_nsset.cc
    src/libfred/registrable_object/nsset/info_nsset.cc
    src/libfred/registrable_object/nsset/info_nsset_data.cc
    src/libfred/registrable_object/nsset/info_nsset_diff.cc
    src/libfred/registrable_object/nsset/info_nsset_impl.cc
    src/libfred/registrable_object/nsset/info_nsset_output.cc
    src/libfred/registrable_object/nsset/transfer_nsset.cc
    src/libfred/registrable_object/nsset/update_nsset.cc
    src/libfred/registrar/check_registrar.cc
    src/libfred/registrar/create_registrar.cc
    src/libfred/registrar/get_registrar_handles.cc
    src/libfred/registrar/get_registrar_zone_credit.cc
    src/libfred/registrar/info_registrar.cc
    src/libfred/registrar/info_registrar_data.cc
    src/libfred/registrar/info_registrar_diff.cc
    src/libfred/registrar/info_registrar_impl.cc
    src/libfred/registrar/info_registrar_output.cc
    src/libfred/registrar/registrar_zone_access.cc
    src/libfred/registrar/registrar_zone_credit.cc
    src/libfred/registrar/zone_credit.cc
    src/libfred/registrar/certification/create_registrar_certification.cc
    src/libfred/registrar/certification/get_registrar_certifications.cc
    src/libfred/registrar/certification/update_registrar_certification.cc
    src/libfred/registrar/group/cancel_registrar_group.cc
    src/libfred/registrar/group/create_registrar_group.cc
    src/libfred/registrar/group/get_registrar_groups.cc
    src/libfred/registrar/group/update_registrar_group.cc
    src/libfred/registrar/group/membership/create_registrar_group_membership.cc
    src/libfred/registrar/group/membership/end_registrar_group_membership.cc
    src/libfred/registrar/group/membership/info_group_membership_by_group.cc
    src/libfred/registrar/group/membership/info_group_membership_by_registrar.cc
    src/libfred/zone/create_zone.cc
    src/libfred/zone/exceptions.cc
    src/libfred/zone/info_zone.cc
    src/libfred/zone/info_zone_data.cc
    src/libfred/zone/update_zone.cc
    src/libfred/zone/util.cc
    src/libfred/zone/zone.cc
    src/libfred/zone_ns/create_zone_ns.cc
    src/libfred/zone_ns/exceptions.cc
    src/libfred/zone_ns/info_zone_ns.cc
    src/libfred/zone_ns/update_zone_ns.cc
    src/libfred/zone_ns/util.cc
    src/libfred/zone_soa/create_zone_soa.cc
    src/libfred/zone_soa/info_zone_soa.cc
    src/libfred/zone_soa/update_zone_soa.cc
    src/util/idn_utils.cc
    src/util/printable.cc
    src/util/random.cc
    src/util/util.cc
    src/util/db/param_query_composition.cc
    src/util/db/value.cc
    src/util/db/psql/psql_connection.cc
    src/util/db/psql/psql_result.cc
    src/util/log/context.cc
    src/util/log/log.cc
    src/util/log/logger.cc
    src/util/log/log_types.cc
    src/util/types/date.cc
    src/util/types/date_interval.cc
    src/util/types/datetime.cc
    src/util/types/datetime_interval.cc
    src/util/types/id.cc)
add_library(fred::library ALIAS fred)

set_target_properties(fred PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PUBLIC_HEADER src/libfred/db_settings.hh)

target_compile_options(fred
    PUBLIC "-ggdb"
    PUBLIC "-grecord-gcc-switches"
    PUBLIC "-Wall"
    PUBLIC "-Wextra"
    PUBLIC "-fdiagnostics-color=auto")

set(PACKAGE libfred)
set(HAVE_BOOST_SERIALIZATION 1)
set(HAVE_LOGGER 1)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
configure_file(fred.pc.in fred.pc @ONLY)

include(CheckIncludeFileCXX)
check_include_file_cxx("mpdecimal.h" FOUND_mpdecimal_h)
if(NOT FOUND_mpdecimal_h)
    message(FATAL_ERROR "Required header mpdecimal.h not found!")
endif()
set(Mpdec_LIBRARY mpdec)

find_package(Threads REQUIRED)
set(Threads_LIBRARIES Threads::Threads)
find_package(PostgreSQL REQUIRED)
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

target_include_directories(fred
    PRIVATE .
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(fred PRIVATE
    Threads::Threads
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_RANDOM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Mpdec_LIBRARY}
    ${PostgreSQL_LIBRARIES})

install(TARGETS fred
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/fred.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_executable(test-libfred libfred.cc)

set_target_properties(test-libfred PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

target_link_libraries(test-libfred PRIVATE
    fred::library
    ${Boost_SYSTEM_LIBRARY})
