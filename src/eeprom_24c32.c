/**
 * @file eeprom_24c32.c
 * @brief Implementation of 24C32 EEPROM driver using NHAL I2C interface
 */

#include "eeprom_24c32.h"
#include "nhal_common.h"
#include <string.h>

static eeprom_24c32_result_t hal_to_eeprom_result(nhal_result_t hal_result)
{
    switch (hal_result) {
        case NHAL_OK:
            return EEPROM_24C32_OK;
        case NHAL_ERR_TIMEOUT:
            return EEPROM_24C32_ERR_WRITE_TIMEOUT;
        case NHAL_ERR_INVALID_ARG:
            return EEPROM_24C32_ERR_INVALID_ARG;
        case NHAL_ERR_NO_RESPONSE:
        case NHAL_ERR_TRANSMISSION_ERROR:
        case NHAL_ERR_HW_FAILURE:
        case NHAL_ERR_BUSY:
        case NHAL_ERR_NOT_INITIALIZED:
        case NHAL_ERR_NOT_CONFIGURED:
        case NHAL_ERR_OTHER:
            return EEPROM_24C32_ERR_I2C_ERROR;
        default:
            return EEPROM_24C32_ERR_I2C_ERROR;
    }
}

eeprom_24c32_result_t eeprom_24c32_init(
    eeprom_24c32_handle_t *handle,
    struct nhal_i2c_context *ctx,
    uint8_t device_address)
{
    if (handle == NULL || ctx == NULL) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    if (device_address > 0x7F) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    handle->ctx = ctx;
    handle->device_address.type = NHAL_I2C_7BIT_ADDR;
    handle->device_address.addr.address_7bit = device_address;

    return EEPROM_24C32_OK;
}

eeprom_24c32_result_t eeprom_24c32_read(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    uint8_t *data,
    size_t length)
{
    if (handle == NULL || data == NULL || length == 0) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    if (address >= EEPROM_24C32_SIZE_BYTES ||
        (address + length) > EEPROM_24C32_SIZE_BYTES) {
        return EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE;
    }

    uint8_t addr_bytes[2] = {
        (uint8_t)((address >> 8) & 0xFF),
        (uint8_t)(address & 0xFF)
    };

    nhal_result_t result = nhal_i2c_master_write_read_reg(
        handle->ctx,
        handle->device_address,
        addr_bytes,
        sizeof(addr_bytes),
        data,
        length
    );

    return hal_to_eeprom_result(result);
}

eeprom_24c32_result_t eeprom_24c32_write_page(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    const uint8_t *data,
    size_t length)
{
    if (handle == NULL || data == NULL || length == 0) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    if (address >= EEPROM_24C32_SIZE_BYTES ||
        (address + length) > EEPROM_24C32_SIZE_BYTES) {
        return EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE;
    }

    if (length > EEPROM_24C32_PAGE_SIZE_BYTES) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    uint16_t page_start = address & ~(EEPROM_24C32_PAGE_SIZE_BYTES - 1);
    if ((address + length) > (page_start + EEPROM_24C32_PAGE_SIZE_BYTES)) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    uint8_t write_buffer[2 + EEPROM_24C32_PAGE_SIZE_BYTES];
    write_buffer[0] = (uint8_t)((address >> 8) & 0xFF);
    write_buffer[1] = (uint8_t)(address & 0xFF);
    memcpy(&write_buffer[2], data, length);

    nhal_result_t result = nhal_i2c_master_write(
        handle->ctx,
        handle->device_address,
        write_buffer,
        2 + length
    );

    return hal_to_eeprom_result(result);
}

eeprom_24c32_result_t eeprom_24c32_write(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    const uint8_t *data,
    size_t length)
{
    if (handle == NULL || data == NULL || length == 0) {
        return EEPROM_24C32_ERR_INVALID_ARG;
    }

    if (address >= EEPROM_24C32_SIZE_BYTES ||
        (address + length) > EEPROM_24C32_SIZE_BYTES) {
        return EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE;
    }

    size_t bytes_written = 0;
    uint16_t current_address = address;
    const uint8_t *current_data = data;

    while (bytes_written < length) {
        uint16_t page_start = current_address & ~(EEPROM_24C32_PAGE_SIZE_BYTES - 1);
        uint16_t bytes_to_page_end = (page_start + EEPROM_24C32_PAGE_SIZE_BYTES) - current_address;
        size_t bytes_to_write = (length - bytes_written) < bytes_to_page_end ?
                               (length - bytes_written) : bytes_to_page_end;

        eeprom_24c32_result_t result = eeprom_24c32_write_page(
            handle,
            current_address,
            current_data,
            bytes_to_write
        );

        if (result != EEPROM_24C32_OK) {
            return result;
        }

        uint32_t elapsed_ms = 0;
        while (!eeprom_24c32_is_ready(handle)) {
            if (elapsed_ms >= EEPROM_24C32_WRITE_CYCLE_TIME_MS) {
                return EEPROM_24C32_ERR_WRITE_TIMEOUT;
            }
            nhal_delay_milliseconds(1);
            elapsed_ms++;
        }

        bytes_written += bytes_to_write;
        current_address += bytes_to_write;
        current_data += bytes_to_write;
    }

    return EEPROM_24C32_OK;
}

bool eeprom_24c32_is_ready(eeprom_24c32_handle_t *handle)
{
    if (handle == NULL) {
        return false;
    }

    uint8_t dummy_data = 0;
    nhal_result_t result = nhal_i2c_master_read(
        handle->ctx,
        handle->device_address,
        &dummy_data,
        1
    );

    return (result == NHAL_OK);
}
