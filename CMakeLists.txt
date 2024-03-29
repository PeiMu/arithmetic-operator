cmake_minimum_required(VERSION 3.21)
project(arithmetic_operator)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")

set(fixed_point_operator_files
        fixed_point_operator/fixed_point_operator.c
)

set(bit_operator_files
        bit_operator/bit_operator.c
)

set(soft_float_operator_files
        soft_float_operator/soft_float_operator.c
)

set(bitwise_and_mod_files
        bitwise_and_mod/non_linear_bin_op.cpp
)

add_library(fixed_point_operator_lib
        ${fixed_point_operator_files}
)

add_library(bit_operator_lib
        ${bit_operator_files}
)

add_library(soft_float_operator_lib
        ${soft_float_operator_files}
)

add_library(bitwise_and_mod_lib
        ${bitwise_and_mod_files}
)

add_executable(arithmetic_operator
        main.c
)

target_link_libraries(arithmetic_operator
        fixed_point_operator_lib
        bit_operator_lib
        soft_float_operator_lib
        bitwise_and_mod_lib
        m
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
        soft_float_operator/gtest.cpp
)

target_link_libraries(
        arithmetic_operator_test
        fixed_point_operator_lib
        bit_operator_lib
        soft_float_operator_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(arithmetic_operator_test)

# softfloat util
add_executable(softfloat_test
        soft_float_operator/util/softfloat_test.c
)

add_executable(union_test
        soft_float_operator/util/union_test.cpp
)
