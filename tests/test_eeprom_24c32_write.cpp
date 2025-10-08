#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test_nhal_i2c_context_stub.h"
#include "nhal_i2c_mock.hpp"

extern "C" {
    #include "eeprom_24c32.h"
}

using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::Truly;

class Eeprom24c32WriteTest : public ::testing::Test {
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

TEST_F(Eeprom24c32WriteTest, WritePageSuccess) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

    EXPECT_CALL(NhalI2cMock::instance(),
                nhal_i2c_master_write(&ctx,
                                      Truly([](const nhal_i2c_address &addr) {
                                          return addr.type == NHAL_I2C_7BIT_ADDR &&
                                                 addr.addr.address_7bit == 0x50;
                                      }),
                                      _, 6))
        .WillOnce(Return(NHAL_OK));

    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, 0, data, 4);

    EXPECT_EQ(result, EEPROM_24C32_OK);
}

TEST_F(Eeprom24c32WriteTest, WritePageNullHandle) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    eeprom_24c32_result_t result = eeprom_24c32_write_page(nullptr, 0, data, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WritePageNullData) {
    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, 0, nullptr, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WritePageZeroLength) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, 0, data, 0);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WritePageTooLarge) {
    uint8_t data[EEPROM_24C32_PAGE_SIZE_BYTES + 1];
    
    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, 0, data, sizeof(data));
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WritePageCrossesBoundary) {
    uint8_t data[16];
    uint16_t address = EEPROM_24C32_PAGE_SIZE_BYTES - 8; // 8 bytes from page end
    
    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, address, data, 16);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WritePageAddressOutOfRange) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    eeprom_24c32_result_t result = eeprom_24c32_write_page(&handle, EEPROM_24C32_SIZE_BYTES, data, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE);
}

TEST_F(Eeprom24c32WriteTest, WriteSuccess) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};

    // Expect write operation
    EXPECT_CALL(NhalI2cMock::instance(), nhal_i2c_master_write(_, _, _, _))
        .WillOnce(Return(NHAL_OK));

    // Expect ready check (polling for write completion)
    EXPECT_CALL(NhalI2cMock::instance(), nhal_i2c_master_read(_, _, _, _))
        .WillOnce(Return(NHAL_OK)); // Ready immediately

    eeprom_24c32_result_t result = eeprom_24c32_write(&handle, 0, data, 4);

    EXPECT_EQ(result, EEPROM_24C32_OK);
}

TEST_F(Eeprom24c32WriteTest, WriteNullHandle) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    eeprom_24c32_result_t result = eeprom_24c32_write(nullptr, 0, data, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WriteNullData) {
    eeprom_24c32_result_t result = eeprom_24c32_write(&handle, 0, nullptr, 4);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, WriteZeroLength) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    eeprom_24c32_result_t result = eeprom_24c32_write(&handle, 0, data, 0);
    
    EXPECT_EQ(result, EEPROM_24C32_ERR_INVALID_ARG);
}

TEST_F(Eeprom24c32WriteTest, IsReadyTrue) {
    EXPECT_CALL(NhalI2cMock::instance(),
                nhal_i2c_master_read(&ctx,
                                     Truly([](const nhal_i2c_address &addr) {
                                         return addr.type == NHAL_I2C_7BIT_ADDR &&
                                                addr.addr.address_7bit == 0x50;
                                     }),
                                     _, 1))
        .WillOnce(Return(NHAL_OK));

    bool ready = eeprom_24c32_is_ready(&handle);

    EXPECT_TRUE(ready);
}

TEST_F(Eeprom24c32WriteTest, IsReadyFalse) {
    EXPECT_CALL(NhalI2cMock::instance(),
                nhal_i2c_master_read(&ctx,
                                     Truly([](const nhal_i2c_address &addr) {
                                         return addr.type == NHAL_I2C_7BIT_ADDR &&
                                                addr.addr.address_7bit == 0x50;
                                     }),
                                     _, 1))
        .WillOnce(Return(NHAL_ERR_OTHER));

    bool ready = eeprom_24c32_is_ready(&handle);
    
    EXPECT_FALSE(ready);
}

TEST_F(Eeprom24c32WriteTest, IsReadyNullHandle) {
    bool ready = eeprom_24c32_is_ready(nullptr);
    
    EXPECT_FALSE(ready);
}
