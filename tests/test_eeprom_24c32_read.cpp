#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test_nhal_i2c_context_stub.h"
#include "nhal_i2c_mock.hpp"

extern "C" {
    #include "eeprom_24c32.h"
}

using ::testing::_;
using ::testing::Return;
using ::testing::SetArrayArgument;
using ::testing::Truly;

class Eeprom24c32ReadTest : public ::testing::Test {
protected:
    void SetUp() override {
        memset(&handle, 0, sizeof(handle));
        memset(&ctx, 0, sizeof(ctx));

        ASSERT_EQ(eeprom_24c32_init(&handle, &ctx, 0x50), EEPROM_24C32_OK);

        testing::Mock::VerifyAndClearExpectations(&NhalI2cMock::instance());
    }

    void TearDown() override {
        testing::Mock::VerifyAndClearExpectations(&NhalI2cMock::instance());
    }

    eeprom_24c32_handle_t handle;
    struct nhal_i2c_context ctx;
};

TEST_F(Eeprom24c32ReadTest, ReadValidData) {
    uint8_t expected_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t read_buffer[4];
    uint16_t address = 0x0100;

    EXPECT_CALL(NhalI2cMock::instance(), nhal_i2c_master_write_read_reg(
        &ctx,
        Truly([](const nhal_i2c_address &addr) {
            return addr.type == NHAL_I2C_7BIT_ADDR && addr.addr.address_7bit == 0x50;
        }),
        _, 2, read_buffer, 4))
        .WillOnce([&](struct nhal_i2c_context* ctx, nhal_i2c_address addr, const uint8_t* reg, size_t reg_len, uint8_t* data, size_t data_len) {
            memcpy(data, expected_data, 4);
            return NHAL_OK;
        });

    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, address, read_buffer, 4);

    EXPECT_EQ(result, EEPROM_24C32_OK);
    EXPECT_EQ(memcmp(read_buffer, expected_data, 4), 0);
}

TEST_F(Eeprom24c32ReadTest, ReadNullHandle) {
    uint8_t buffer[4];
    
    eeprom_24c32_result_t result = eeprom_24c32_read(nullptr, 0, buffer, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32ReadTest, ReadNullBuffer) {
    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, 0, nullptr, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32ReadTest, ReadZeroLength) {
    uint8_t buffer[4];
    
    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, 0, buffer, 0);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32ReadTest, ReadAddressOutOfRange) {
    uint8_t buffer[4];
    
    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, EEPROM_24C32_SIZE_BYTES, buffer, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE);
}

TEST_F(Eeprom24c32ReadTest, ReadLengthExceedsSize) {
    uint8_t buffer[4];
    
    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, EEPROM_24C32_SIZE_BYTES - 2, buffer, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE);
}

TEST_F(Eeprom24c32ReadTest, ReadI2CError) {
    uint8_t buffer[4];

    EXPECT_CALL(NhalI2cMock::instance(), nhal_i2c_master_write_read_reg(_, _, _, _, _, _))
        .WillOnce(Return(NHAL_ERR_OTHER));

    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, 0, buffer, 4);

    EXPECT_EQ(result, EEPROM_24C32_ERR_I2C_ERROR);
}

TEST_F(Eeprom24c32ReadTest, ReadTimeout) {
    uint8_t buffer[4];

    EXPECT_CALL(NhalI2cMock::instance(), nhal_i2c_master_write_read_reg(_, _, _, _, _, _))
        .WillOnce(Return(NHAL_ERR_TIMEOUT));

    eeprom_24c32_result_t result = eeprom_24c32_read(&handle, 0, buffer, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_WRITE_TIMEOUT);
}
