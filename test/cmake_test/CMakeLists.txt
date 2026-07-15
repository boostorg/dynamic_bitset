# Copyright 2021-2024 Alexander Grund
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_subdir_test LANGUAGES CXX)

# Those 2 should work the same
# while using find_package for the installed Boost avoids the need to manually specify dependencies
if(BOOST_CI_INSTALL_TEST)
    # dynamic_bitset is header-only. B2 does not (yet) create/install CMake
    # targets for header-only libraries, so when installed by B2 use the
    # generic Boost::headers target; when installed by CMake the per-library
    # config is available as usual.
    if(BOOST_CI_INSTALLED_BY STREQUAL "B2")
        find_package(Boost REQUIRED)
        if(CMAKE_VERSION VERSION_LESS 3.18)
            add_library(Boost_dynamic_bitset INTERFACE)
            target_link_libraries(Boost_dynamic_bitset INTERFACE Boost::headers)
            add_library(Boost::dynamic_bitset ALIAS Boost_dynamic_bitset)
        else()
            add_library(Boost::dynamic_bitset ALIAS Boost::headers)
        endif()
    else()
        find_package(boost_dynamic_bitset REQUIRED)
    endif()
else()
    set(BOOST_INCLUDE_LIBRARIES dynamic_bitset)
    add_subdirectory(../../../.. deps/boost EXCLUDE_FROM_ALL)
endif()

add_executable(main main.cpp)
target_link_libraries(main Boost::dynamic_bitset)

enable_testing()
add_test(NAME main COMMAND main)
