cmake_minimum_required(VERSION 2.8)

project(CMakeLists CXX)

set(CMakeLists_VERSION_MAJOR 0)
set(CMakeLists_VERSION_MINOR 1)
set(CMakeLists_VERSION_PATCH 0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++14 -ggdb -Wall -Wextra -fdiagnostics-color=auto")

#add_definitions(-std=c++14)

find_package(Boost COMPONENTS date_time program_options REQUIRED)

add_executable(CMakeLists
	PRIVATE CMakeLists.cpp
)

target_link_libraries(CMakeLists ${Boost_LIBRARIES})

include_directories("." PRIVATE)

pkg_search_module(GETDNS REQUIRED getdns>=1.2.1)
target_include_directories(CMakeLists PUBLIC ${GETDNS_INCLUDE_DIRS})
target_compile_options(CMakeLists PUBLIC ${GETDNS_CXXFLAGS})
target_link_libraries(CMakeLists ${GETDNS_LIBRARIES})

# tests
include(CTest)

# application runs
add_test(CMakeListsRuns CMakeLists)

# application accepts --help option
add_test(CMakeListsHelps CMakeLists "--help")

# application accepts --all option
add_test(CMakeListsAll CMakeLists "--all")