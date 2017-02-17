cmake_minimum_required(VERSION 3.0)
project(templ CXX)

option(TEMPL_ENABLE_TESTS "Include compile-time tests for templ" ON)
option(TEMPL_ENABLE_INSTALL "Add install targets for templ headers" OFF)

include(CheckCXXSourceCompiles)

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


set(CMAKE_REQUIRED_FLAGS
        ${CMAKE_REQUIRED_FLAGS}
        ${CMAKE_CXX14_STANDARD_COMPILE_OPTION}
        ${CMAKE_CXX_FLAGS})

set(CMAKE_REQUIRED_INCLUDES
        ${CMAKE_REQUIRED_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

check_cxx_source_compiles("
    template<int, class>
    struct A {};

    template<class...>
    struct B : A<0, int>, A<1, char> {};

    template<int ...Is, class ...Ts>
    B<Ts...> test(A<Is, Ts>...);

    decltype(test<0>(B<>{})) v;

    int main() {}
" TEMPL_CAN_DEDUCT_SELECT)

if(TEMPL_CAN_DEDUCT_SELECT)
    target_compile_definitions(templ INTERFACE
            TEMPL_CAN_DEDUCT_SELECT)
endif()

if(TEMPL_ENABLE_TESTS)
    add_executable(templ_test
            test/main.cpp
            test/test_algorithm.hpp
            include/templ/templ.hpp
            include/templ/algorithm.hpp
            include/templ/basic.hpp
            include/templ/map.hpp
            include/templ/skel.hpp
            include/templ/tstring.hpp
            test/test_common.hpp)

    set_target_properties(templ_test PROPERTIES
            CXX_STANDARD 14
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF)

    target_link_libraries(templ_test templ)
endif()