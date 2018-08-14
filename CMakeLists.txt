cmake_minimum_required(VERSION 3.5)

project(fred
    VERSION 0.1.0
    LANGUAGES CXX)

if("CXX" IN_LIST languages)
    message("C++ enabled")
endif()

include(GNUInstallDirs)

add_library(fred SHARED
    src/libfred/opcontext.cc
    src/libfred/registrable_object/contact/info_contact.cc
    src/libfred/registrable_object/contact/info_contact_data.cc
    src/libfred/registrable_object/contact/info_contact_diff.cc
    src/libfred/registrable_object/contact/info_contact_impl.cc
    src/libfred/registrable_object/contact/info_contact_output.cc
    src/libfred/registrable_object/contact/place_address.cc
    src/util/printable.cc
    src/util/util.cc
    src/util/db/param_query_composition.cc
    src/util/db/value.cc
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
    ${Boost_REGEX_LIBRARY}
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
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${PostgreSQL_LIBRARIES})
