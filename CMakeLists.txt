cmake_minimum_required(VERSION 3.5)

get_directory_property(HAS_PARENT "PARENT_DIRECTORY")
if(NOT HAS_PARENT)
    set(IS_TOP_LEVEL TRUE)
else()
    set(IS_TOP_LEVEL FALSE)
endif()

# build options

option(LIBFRED_BUILD_TESTS "Build ${PACKAGE_NAME} tests" ${IS_TOP_LEVEL})

# Set CMake policies to support later version behaviour
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # option() honors variables already set

# compile options

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Specifies the build type. Possible values are empty, Debug, Release, RelWithDebInfo, MinSizeRel, ...")
# CMAKE_<LANG>_FLAGS_<CONFIG> new in cmake version 3.11
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
option(THREADS_PREFER_PTHREAD_FLAG "The -pthread compiler and linker flag preference." TRUE)
option(CMAKE_EXPORT_COMPILE_COMMANDS "If enabled, generates a compile_commands.json file containing the exact compiler calls." ON)
option(USE_LIBLOG "Library uses LibLog for application logging." TRUE)
option(FMT_HEADER_ONLY "Use fmt library in header-only manner." OFF) # significantly reduce build time

# project details

include("cmake/package.cmake")

package(fred-libfred FULL_VERSION "6.0.0")

project(LibFred
    VERSION ${PACKAGE_VERSION}
    LANGUAGES CXX)

# externals

include("cmake/package_dependencies.cmake")

add_package_dependencies(FROM_FILE "dependencies.txt")

# dependencies

include(CheckIncludeFileCXX)
check_include_file_cxx("mpdecimal.h" FOUND_mpdecimal_h)
if(NOT FOUND_mpdecimal_h)
    message(FATAL_ERROR "Required header mpdecimal.h not found!")
endif()
set(Mpdec_LIBRARY mpdec)

find_package(Threads REQUIRED)
set(Threads_LIBRARIES Threads::Threads)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
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

# configuration files
if(USE_LIBLOG)
    set(HAVE_LOGGER "1")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(fred.pc.in fred.pc @ONLY)

# build

function(set_common_properties_on_targets)
    foreach(target_name ${ARGN})
        message(STATUS "setting common properties on: " ${target_name})
        set_target_properties(${target_name} PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED YES
            CXX_EXTENSIONS NO
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        target_include_directories(${target_name}
            PUBLIC
                ${CMAKE_CURRENT_SOURCE_DIR}/src
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_BINARY_DIR})
        target_compile_options(${target_name}
            PUBLIC
                $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-copy>
            PRIVATE
                $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -ggdb -grecord-gcc-switches>)
        if(USE_LIBLOG)
            target_compile_definitions(${target_name} PUBLIC HAVE_LOGGER=1)
        endif()
    endforeach()
endfunction()

add_library(fred STATIC
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
src/libfred/registrar/certification/delete_registrar_certification.cc
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
src/libfred/registrar/zone_access/delete_registrar_zone_access.cc
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

add_library(Fred::library ALIAS fred)

target_link_libraries(fred
PUBLIC
    Threads::Threads
    Boost::system
    Boost::thread
    Boost::date_time
    Boost::random
    Boost::regex
    LibLog::library
    LibPg::library
    ${Mpdec_LIBRARY}
    ${LIBIDN_LIBRARIES}
    ${LIBCRYPTO_LIBRARIES})

set_common_properties_on_targets(
    fred)

# tests

if(LIBFRED_BUILD_TESTS)
    add_subdirectory(test)
endif()

# installation

include(GNUInstallDirs)

install(TARGETS fred
    DESTINATION ${CMAKE_INSTALL_SLIBDIR})

if(IS_TOP_LEVEL)
    add_custom_target(uninstall
        COMMAND xargs -L10 rm -v < ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
        COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt | xargs -L1 dirname | sort | uniq | xargs -L10 rmdir -p --ignore-fail-on-non-empty)
endif()

# dist

include("cmake/package_dist.cmake")
package_dist(PACKAGE_NAME "${PROJECT_NAME}"
             PACKAGE_PREFIX "${LIBFRED_PREFIX}"
             PACKAGE_TARNAME "${PACKAGE_TARNAME}")

list_package_dependencies_recursive("${PACKAGE_NAME}")
