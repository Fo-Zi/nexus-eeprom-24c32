#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "nhal_i2c_mock.hpp"

extern "C" {
    #include "eeprom_24c32.h"
}

class Eeprom24c32InitTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test fixtures
        memset(&handle, 0, sizeof(handle));
        memset(&ctx, 0, sizeof(ctx));
        
        // Reset mock for each test
        testing::Mock::VerifyAndClearExpectations(&NhalI2cMock::instance());
    }

    void TearDown() override {
        testing::Mock::VerifyAndClearExpectations(&NhalI2cMock::instance());
    }

    eeprom_24c32_handle_t handle;
    struct nhal_i2c_context ctx;
};

TEST_F(Eeprom24c32InitTest, InitWithValidParameters) {
    uint8_t device_address = 0x50;
    nhal_timeout_ms timeout = 1000;

    eeprom_24c32_result_t result = eeprom_24c32_init(&handle, &ctx, device_address, timeout);

    EXPECT_EQ(result, EEPROM_24C32_OK);
    EXPECT_EQ(handle.ctx, &ctx);
    EXPECT_EQ(handle.device_address, device_address);
    EXPECT_EQ(handle.timeout_ms, timeout);
}

TEST_F(Eeprom24c32InitTest, InitWithNullHandle) {
    uint8_t device_address = 0x50;
    nhal_timeout_ms timeout = 1000;

    eeprom_24c32_result_t result = eeprom_24c32_init(nullptr, &ctx, device_address, timeout);

    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32InitTest, InitWithNullContext) {
    uint8_t device_address = 0x50;
    nhal_timeout_ms timeout = 1000;

    eeprom_24c32_result_t result = eeprom_24c32_init(&handle, nullptr, device_address, timeout);

    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32InitTest, InitWithBothNullParameters) {
    uint8_t device_address = 0x50;
    nhal_timeout_ms timeout = 1000;

    eeprom_24c32_result_t result = eeprom_24c32_init(nullptr, nullptr, device_address, timeout);

    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}