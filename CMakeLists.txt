cmake_minimum_required(VERSION 3.21)
project(arithmetic_operator)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-g -Wall")

set(fixed_point_operator_files
        fixed_point_operator/fixed_point_operator.cpp
)

set(bit_operator_files
        bit_operator/bit_operator.cpp
)

add_library(fixed_point_operator_lib
        ${fixed_point_operator_files}
)

add_library(bit_operator_lib
        ${bit_operator_files}
)

add_executable(arithmetic_operator
        main.cpp
)

target_link_libraries(arithmetic_operator
        fixed_point_operator_lib
        bit_operator_lib
)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        arithmetic_operator_test
        fixed_point_operator/gtest.cpp
)

target_link_libraries(
        arithmetic_operator_test
        fixed_point_operator_lib
        bit_operator_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(arithmetic_operator_test)
