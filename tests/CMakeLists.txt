cmake_minimum_required(VERSION 3.10)
project(eeprom_24c32_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        ../../hal-interface/include
)

# Add only the I2C mock (avoid broken UART/SPI mocks)
add_library(nhal_i2c_mock_only
    ../../hal-interface/testing/gtest_mocks/src/nhal_i2c_mock.cpp
)

target_include_directories(nhal_i2c_mock_only
    PUBLIC
        ../../hal-interface/testing/gtest_mocks/include
        ../../hal-interface/include
)

target_link_libraries(nhal_i2c_mock_only
    PUBLIC
        GTest::gmock
        GTest::gtest
)

# Create test executable
add_executable(test_eeprom_24c32
    test_eeprom_24c32_init.cpp
)

target_link_libraries(test_eeprom_24c32
    PRIVATE
        eeprom_24c32_lib
        nhal_i2c_mock_only
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

target_include_directories(test_eeprom_24c32
    PRIVATE
        ../include
        ../../hal-interface/include
        ../../hal-interface/testing/gtest_mocks/include
)

# Enable testing
enable_testing()
add_test(NAME EepromInitTests COMMAND test_eeprom_24c32)