cmake_minimum_required(VERSION 3.0)
project(templ CXX)

option(TEMPL_ENABLE_TESTS "Include compile-time tests for templ" ON)
option(TEMPL_ENABLE_INSTALL "Add install targets for templ headers" OFF)

add_library(templ INTERFACE)

target_include_directories(templ INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/templ>)

if(TEMPL_ENABLE_INSTALL)
    install(TARGETS templ EXPORT templ_export)
    install(EXPORT templ_export NAMESPACE Upstream::
        DESTINATION lib/cmake/templ)

    install(DIRECTORY include/templ/ DESTINATION include/templ
            FILES_MATCHING PATTERN "*.hpp")
endif()

if(TEMPL_ENABLE_TESTS)
    add_executable(templ_test
            test/main.cpp
            test/cases/algorithm.hpp
            include/templ/templ.hpp
            include/templ/algorithm.hpp
            include/templ/basic.hpp
            include/templ/map.hpp
            include/templ/skel.hpp
            include/templ/tstring.hpp)

    target_link_libraries(templ_test templ)
endif()