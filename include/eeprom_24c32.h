/**
 * @file eeprom_24c32.h
 * @brief Driver for 24C32 EEPROM using HAL I2C interface
 *
 * This driver provides functions to read and write data to/from
 * a 24C32 EEPROM (32Kbit/4KB) using the HAL I2C interface.
 */
#ifndef EEPROM_24C32_H
#define EEPROM_24C32_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "hal_i2c_master.h"
#include "hal_i2c_types.h"

#define EEPROM_24C32_SIZE_BYTES         4096    /**< Total EEPROM size in bytes */
#define EEPROM_24C32_PAGE_SIZE_BYTES    32      /**< Page size for write operations */
#define EEPROM_24C32_WRITE_CYCLE_TIME_MS 5     /**< Maximum write cycle time */

typedef enum {
    EEPROM_24C32_OK = 0,                /**< Operation completed successfully */
    EEPROM_24C32_ERR_INVALID_ARG,       /**< Invalid arguments provided */
    EEPROM_24C32_ERR_ADDRESS_OUT_OF_RANGE, /**< Address exceeds EEPROM size */
    EEPROM_24C32_ERR_I2C_ERROR,         /**< I2C communication error */
    EEPROM_24C32_ERR_WRITE_TIMEOUT,     /**< Write operation timed out */
} eeprom_24c32_result_t;

typedef struct {
    struct hal_i2c_context *i2c_ctx;   /**< HAL I2C context */
    hal_timeout_ms timeout_ms;          /**< I2C operation timeout */
} eeprom_24c32_handle_t;

/**
 * @brief Initialize EEPROM 24C32 driver
 *
 * @param handle Pointer to EEPROM handle structure
 * @param i2c_ctx Initialized HAL I2C context
 * @param device_address 7-bit I2C device address (typically 0x50)
 * @param timeout_ms Timeout for I2C operations
 * @return eeprom_24c32_result_t Result of initialization
 */
eeprom_24c32_result_t eeprom_24c32_init(
    eeprom_24c32_handle_t *handle,
    struct hal_i2c_context *i2c_ctx,
    hal_timeout_ms timeout_ms
);

/**
 * @brief Read data from EEPROM
 *
 * @param handle Pointer to initialized EEPROM handle
 * @param address Starting address to read from (0-4095)
 * @param data Buffer to store read data
 * @param length Number of bytes to read
 * @return eeprom_24c32_result_t Result of read operation
 */
eeprom_24c32_result_t eeprom_24c32_read(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    uint8_t *data,
    size_t length
);

/**
 * @brief Write data to EEPROM
 *
 * This function handles page boundaries automatically and includes
 * write cycle delays as needed.
 *
 * @param handle Pointer to initialized EEPROM handle
 * @param address Starting address to write to (0-4095)
 * @param data Data to write
 * @param length Number of bytes to write
 * @return eeprom_24c32_result_t Result of write operation
 */
eeprom_24c32_result_t eeprom_24c32_write(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    const uint8_t *data,
    size_t length
);

/**
 * @brief Write a single page to EEPROM
 *
 * Internal function for writing up to one page (32 bytes) at a time.
 * Address must be page-aligned or within the same page.
 *
 * @param handle Pointer to initialized EEPROM handle
 * @param address Starting address within a page
 * @param data Data to write
 * @param length Number of bytes to write (max 32)
 * @return eeprom_24c32_result_t Result of page write operation
 */
eeprom_24c32_result_t eeprom_24c32_write_page(
    eeprom_24c32_handle_t *handle,
    uint16_t address,
    const uint8_t *data,
    size_t length
);

/**
 * @brief Check if EEPROM is ready for operations
 *
 * This function can be used to poll the EEPROM after a write operation
 * to determine when the next operation can be performed.
 *
 * @param handle Pointer to initialized EEPROM handle
 * @return true if EEPROM is ready, false if busy
 */
bool eeprom_24c32_is_ready(eeprom_24c32_handle_t *handle);

#endif /* EEPROM_24C32_H */
