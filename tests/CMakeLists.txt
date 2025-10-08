cmake_minimum_required(VERSION 3.10)
project(eeprom_24c32_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Building with code coverage enabled")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()
# HAL interface path - easily configurable
set(HAL_INTERFACE_PATH "../../hal-interface" CACHE STRING "Path to hal-interface directory")

# Find required packages
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Add the main eeprom driver source
add_library(eeprom_24c32_lib
    ../src/eeprom_24c32.c
)

target_include_directories(eeprom_24c32_lib
    PUBLIC
        ../include
        ${HAL_INTERFACE_PATH}/include
)

# Add the specific HAL mocks used by the eeprom driver tests
add_library(nhal_eeprom_hal_mocks
    ${HAL_INTERFACE_PATH}/testing/gtest_mocks/src/nhal_i2c_mock.cpp
    ${HAL_INTERFACE_PATH}/testing/gtest_mocks/src/nhal_common_mock.cpp
)

target_include_directories(nhal_eeprom_hal_mocks
    PUBLIC
        ${HAL_INTERFACE_PATH}/testing/gtest_mocks/include
        ${HAL_INTERFACE_PATH}/include
)

target_link_libraries(nhal_eeprom_hal_mocks
    PUBLIC
        GTest::gmock
        GTest::gtest
)

# Create test executable
add_executable(test_eeprom_24c32
    test_eeprom_24c32_init.cpp
    test_eeprom_24c32_read.cpp
    test_eeprom_24c32_write.cpp
)

target_link_libraries(test_eeprom_24c32
    PRIVATE
        eeprom_24c32_lib
        nhal_eeprom_hal_mocks
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

target_include_directories(test_eeprom_24c32
    PRIVATE
        ../include
        ${HAL_INTERFACE_PATH}/include
        ${HAL_INTERFACE_PATH}/testing/gtest_mocks/include
)

# Enable testing
enable_testing()
add_test(NAME EepromInitTests COMMAND test_eeprom_24c32)

if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "Found lcov and genhtml, coverage targets available")

        set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)
        set(LCOV_IGNORE "")

        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${COVERAGE_DIR}
            COMMAND ${LCOV_PATH} --directory . --zerocounters --quiet || true
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV_PATH} --directory . --capture --output-file ${COVERAGE_DIR}/coverage.info --quiet
            COMMAND ${LCOV_PATH} --remove ${COVERAGE_DIR}/coverage.info '/usr/*' '*/tests/*' '*/hal-interface/testing/gtest_mocks/*' --output-file ${COVERAGE_DIR}/coverage.info --quiet
            COMMAND ${GENHTML_PATH} --demangle-cpp -o ${COVERAGE_DIR}/html ${COVERAGE_DIR}/coverage.info --quiet
            COMMAND ${LCOV_PATH} --list ${COVERAGE_DIR}/coverage.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS test_eeprom_24c32
            COMMENT "Generating complete coverage report..."
        )
    else()
        message(WARNING "lcov or genhtml not found, coverage targets not available")
    endif()
endif()
