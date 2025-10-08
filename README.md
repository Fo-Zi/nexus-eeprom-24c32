![GitHub License](https://img.shields.io/github/license/Fo-Zi/nexus-eeprom-24c32?color=lightgrey)
![GitHub Release](https://img.shields.io/github/v/release/Fo-Zi/nexus-eeprom-24c32?color=brightgreen)
[![CI](https://github.com/Fo-Zi/nexus-eeprom-24c32/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/Fo-Zi/nexus-eeprom-24c32/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/fo-zi/nexus-eeprom-24c32/branch/main/graph/badge.svg)](https://codecov.io/gh/fo-zi/nexus-eeprom-24c32)
[![Dependency](https://img.shields.io/badge/depends%20on-nexus--hal--interface%20v0.6.2-orange)](https://github.com/Fo-Zi/nexus-hal-interface/tree/v0.6.2)

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
```

## Usage

1. Initialize an NHAL I2C context
2. Initialize the EEPROM handle with `eeprom_24c32_init()`
3. Use `eeprom_24c32_read()` and `eeprom_24c32_write()` for data operations

See the header file for detailed function documentation.

## Dependencies

- NHAL I2C interface (v0.6.2 or newer)
- Standard C library

## Development

### Testing

Run the unit tests:

```bash
make run_unit_tests
```

### Code Coverage

Generate a local coverage report:

```bash
make run_coverage
```

The HTML report will be generated at `tests/build-coverage/coverage/html/index.html`.

#### Coverage Requirements
- `lcov` and `genhtml`
- GCC toolchain with coverage support

Install on Ubuntu/Debian:
```bash
sudo apt install lcov
```

### Available Makefile Targets

- `make config_tests` – Configure the CMake build for unit tests
- `make run_unit_tests` – Build and execute unit tests
- `make clean_unit_tests` – Remove the unit-test build directory
- `make config_coverage` – Configure a coverage-enabled build
- `make run_coverage` – Build, run tests, and generate coverage report
- `make clean_coverage` – Remove the coverage build directory
- `make update_deps` – Update West-managed dependencies
