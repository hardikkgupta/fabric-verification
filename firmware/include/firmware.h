#pragma once

#include <stdint.h>
#include <stdbool.h>

// Hardware register definitions
#define LINK_STATUS_REG    0x10000000
#define LINK_CONTROL_REG   0x10000004
#define ERROR_STATUS_REG   0x10000008
#define ERROR_MASK_REG     0x1000000C
#define SELF_TEST_REG      0x10000010

// Link status bits
#define LINK_UP            (1 << 0)
#define LINK_ACTIVE        (1 << 1)
#define LINK_ERROR         (1 << 2)
#define LINK_OVERFLOW      (1 << 3)

// Link control bits
#define LINK_RESET         (1 << 0)
#define LINK_ENABLE        (1 << 1)
#define LINK_TEST_MODE     (1 << 2)
#define LINK_LOOPBACK      (1 << 3)

// Error status bits
#define ERROR_CRC          (1 << 0)
#define ERROR_TIMEOUT      (1 << 1)
#define ERROR_OVERFLOW     (1 << 2)
#define ERROR_UNDERFLOW    (1 << 3)

// Function declarations
void firmware_init(void);
void link_init(void);
void error_handler(void);
void self_test(void);
void process_packet(void);

// Assembly function declarations
void enable_interrupts(void);
void disable_interrupts(void);
void save_context(void);
void restore_context(void);

// RTOS task declarations
void link_monitor_task(void);
void error_handler_task(void);
void self_test_task(void);
void packet_processor_task(void);

// Utility functions
static inline void write_reg(uint32_t addr, uint32_t value) {
    volatile uint32_t* reg = (volatile uint32_t*)addr;
    *reg = value;
}

static inline uint32_t read_reg(uint32_t addr) {
    volatile uint32_t* reg = (volatile uint32_t*)addr;
    return *reg;
}

// Error handling
typedef enum {
    ERROR_NONE = 0,
    ERROR_LINK_DOWN,
    ERROR_CRC_FAIL,
    ERROR_TIMEOUT,
    ERROR_OVERFLOW,
    ERROR_UNDERFLOW
} error_code_t;

// Packet structure
typedef struct {
    uint32_t header;
    uint8_t payload[64];
    uint32_t crc;
} packet_t;

// Link statistics
typedef struct {
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t errors_detected;
    uint32_t crc_errors;
    uint32_t timeout_errors;
} link_stats_t;

// Global variables
extern link_stats_t g_link_stats;
extern bool g_link_initialized;
extern error_code_t g_last_error; 