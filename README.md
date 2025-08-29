# EEPROM 24C32 Driver

A C driver for the 24C32 EEPROM (32Kbit/4KB) using the NHAL I2C interface.

## Overview

This driver provides basic read/write functionality for the 24C32 EEPROM memory device. It handles I2C communication, page boundary management, and write cycle timing automatically.

## Hardware

- **Device**: 24C32 EEPROM
- **Capacity**: 4KB (4,096 bytes)
- **Interface**: I2C
- **Page Size**: 32 bytes
- **Typical Address**: 0x50

## Features

- Read/write operations with automatic page handling
- Write cycle timing management
- Error reporting and validation
- Integration with NHAL I2C abstraction layer

## Building

```bash
# Set up for tests
make config_tests

# Run unit tests
make run_unit_tests

# Full CI pipeline
make ci_local
```

## Usage

1. Initialize an NHAL I2C context
2. Initialize the EEPROM handle with `eeprom_24c32_init()`
3. Use `eeprom_24c32_read()` and `eeprom_24c32_write()` for data operations

See the header file for detailed function documentation.

## Dependencies

- NHAL I2C interface
- Standard C library
