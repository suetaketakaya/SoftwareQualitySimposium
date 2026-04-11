cmake_minimum_required(VERSION 3.14)
project(sakura_macos_test CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Google Test
find_package(GTest REQUIRED)

# Include paths: compat headers take priority
include_directories(
    ${CMAKE_SOURCE_DIR}/compat          # windows_compat.h, sakura_compat.h, StdAfx.h
    ${CMAKE_SOURCE_DIR}/compat          # sakura_core/xxx/yyy.h shims
)

# =============================================================================
# Source files (extracted wrappers of sakura-editor pure functions)
# =============================================================================
set(SRC_FORMAT      ${CMAKE_SOURCE_DIR}/src/format_wrapper.cpp)
set(SRC_CWORDPARSE  ${CMAKE_SOURCE_DIR}/src/cwordparse_wrapper.cpp)
set(SRC_CONVERT     ${CMAKE_SOURCE_DIR}/src/convert_wrapper.cpp)

# =============================================================================
# Test executables
# =============================================================================

# Test: format.cpp (GetDateTimeFormat, ParseVersion, CompareVersion)
add_executable(test_format
    ${SRC_FORMAT}
    ${CMAKE_SOURCE_DIR}/tests/test-format-generated.cpp
    ${CMAKE_SOURCE_DIR}/tests/test-format-additional.cpp
)
target_link_libraries(test_format GTest::gtest GTest::gtest_main)

# Test: CWordParse.cpp (IsMailAddress, WhatKindOfTwoChars, WhatKindOfTwoChars4KW)
add_executable(test_cwordparse
    ${SRC_CWORDPARSE}
    ${CMAKE_SOURCE_DIR}/tests/test-cwordparse-generated.cpp
    ${CMAKE_SOURCE_DIR}/tests/test-cwordparse-additional.cpp
)
target_link_libraries(test_cwordparse GTest::gtest GTest::gtest_main)

# Test: convert_util.cpp (Convert_ZeneisuToHaneisu, Convert_HaneisuToZeneisu)
add_executable(test_convert
    ${SRC_CONVERT}
    ${CMAKE_SOURCE_DIR}/tests/test-convert-generated.cpp
    ${CMAKE_SOURCE_DIR}/tests/test-convert-additional.cpp
)
target_link_libraries(test_convert GTest::gtest GTest::gtest_main)

# =============================================================================
# Enable CTest
# =============================================================================
enable_testing()
add_test(NAME FormatTests     COMMAND test_format)
add_test(NAME CWordParseTests COMMAND test_cwordparse)
add_test(NAME ConvertTests    COMMAND test_convert)
