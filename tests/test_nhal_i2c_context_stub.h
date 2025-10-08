/**
 * @file test_nhal_i2c_context_stub.h
 * @brief Minimal struct definitions for NHAL opaque types used in unit tests.
 *
 * The HAL interface only forward declares these types; tests that need to
 * instantiate them must provide concrete, implementation-agnostic structs.
 * A single dummy field satisfies size requirements for the mocks.
 */
#ifndef TEST_NHAL_I2C_CONTEXT_STUB_H
#define TEST_NHAL_I2C_CONTEXT_STUB_H

#include <stdint.h>

struct nhal_i2c_context {
    uint8_t placeholder;
};

#endif /* TEST_NHAL_I2C_CONTEXT_STUB_H */
