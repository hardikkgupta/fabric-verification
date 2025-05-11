#include "firmware.h"
#include "rtos.h"

// Global variables
link_stats_t g_link_stats = {0};
bool g_link_initialized = false;
error_code_t g_last_error = ERROR_NONE;

// RTOS task handles
static task_handle_t g_link_monitor_task;
static task_handle_t g_error_handler_task;
static task_handle_t g_self_test_task;
static task_handle_t g_packet_processor_task;

void firmware_init(void) {
    // Initialize hardware
    disable_interrupts();
    
    // Reset all registers
    write_reg(LINK_CONTROL_REG, LINK_RESET);
    write_reg(ERROR_MASK_REG, 0);
    write_reg(ERROR_STATUS_REG, 0);
    
    // Initialize link
    link_init();
    
    // Create RTOS tasks
    g_link_monitor_task = create_task(link_monitor_task, "LinkMonitor", 512, 3);
    g_error_handler_task = create_task(error_handler_task, "ErrorHandler", 512, 2);
    g_self_test_task = create_task(self_test_task, "SelfTest", 512, 1);
    g_packet_processor_task = create_task(packet_processor_task, "PacketProc", 512, 4);
    
    // Enable interrupts
    enable_interrupts();
}

void link_init(void) {
    // Reset link
    write_reg(LINK_CONTROL_REG, LINK_RESET);
    
    // Wait for reset to complete
    while (read_reg(LINK_STATUS_REG) & LINK_UP) {
        // Wait
    }
    
    // Enable link
    write_reg(LINK_CONTROL_REG, LINK_ENABLE);
    
    // Wait for link to come up
    while (!(read_reg(LINK_STATUS_REG) & LINK_UP)) {
        // Wait
    }
    
    // Enable error detection
    write_reg(ERROR_MASK_REG, ERROR_CRC | ERROR_TIMEOUT | ERROR_OVERFLOW | ERROR_UNDERFLOW);
    
    g_link_initialized = true;
}

void error_handler(void) {
    uint32_t error_status = read_reg(ERROR_STATUS_REG);
    
    if (error_status & ERROR_CRC) {
        g_last_error = ERROR_CRC_FAIL;
        g_link_stats.crc_errors++;
    }
    else if (error_status & ERROR_TIMEOUT) {
        g_last_error = ERROR_TIMEOUT;
        g_link_stats.timeout_errors++;
    }
    else if (error_status & ERROR_OVERFLOW) {
        g_last_error = ERROR_OVERFLOW;
    }
    else if (error_status & ERROR_UNDERFLOW) {
        g_last_error = ERROR_UNDERFLOW;
    }
    
    g_link_stats.errors_detected++;
    
    // Clear error status
    write_reg(ERROR_STATUS_REG, error_status);
    
    // If too many errors, reset link
    if (g_link_stats.errors_detected > 1000) {
        link_init();
    }
}

void self_test(void) {
    // Enable test mode
    write_reg(LINK_CONTROL_REG, LINK_TEST_MODE | LINK_LOOPBACK);
    
    // Send test pattern
    packet_t test_packet = {0};
    test_packet.header = 0xAA55AA55;
    for (int i = 0; i < sizeof(test_packet.payload); i++) {
        test_packet.payload[i] = i & 0xFF;
    }
    
    // Calculate CRC (simplified)
    test_packet.crc = 0x12345678;
    
    // Send packet
    process_packet();
    
    // Disable test mode
    write_reg(LINK_CONTROL_REG, LINK_ENABLE);
}

void process_packet(void) {
    if (!g_link_initialized) {
        return;
    }
    
    // Check for packet availability
    if (read_reg(LINK_STATUS_REG) & LINK_ACTIVE) {
        // Process packet
        g_link_stats.packets_received++;
        
        // Check for errors
        if (read_reg(LINK_STATUS_REG) & LINK_ERROR) {
            error_handler();
        }
    }
}

// RTOS task implementations
void link_monitor_task(void) {
    while (1) {
        // Monitor link status
        if (!(read_reg(LINK_STATUS_REG) & LINK_UP)) {
            g_last_error = ERROR_LINK_DOWN;
            link_init();
        }
        
        // Update statistics
        if (read_reg(LINK_STATUS_REG) & LINK_ACTIVE) {
            g_link_stats.packets_sent++;
        }
        
        // Sleep for 100ms
        rtos_delay(100);
    }
}

void error_handler_task(void) {
    while (1) {
        // Check for errors
        if (read_reg(ERROR_STATUS_REG)) {
            error_handler();
        }
        
        // Sleep for 50ms
        rtos_delay(50);
    }
}

void self_test_task(void) {
    while (1) {
        // Run self-test every 5 seconds
        self_test();
        rtos_delay(5000);
    }
}

void packet_processor_task(void) {
    while (1) {
        // Process packets
        process_packet();
        
        // Sleep for 10ms
        rtos_delay(10);
    }
} 