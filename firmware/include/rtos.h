#pragma once

#include <stdint.h>
#include <stdbool.h>

// Task handle type
typedef void* task_handle_t;

// Task priority levels
typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_CRITICAL = 4
} task_priority_t;

// Task states
typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_BLOCKED,
    TASK_SUSPENDED
} task_state_t;

// Task control block
typedef struct {
    char name[16];
    void (*entry_point)(void);
    uint32_t stack_size;
    task_priority_t priority;
    task_state_t state;
    uint32_t stack_pointer;
    uint32_t* stack;
    uint32_t sleep_ticks;
} task_control_block_t;

// RTOS functions
task_handle_t create_task(void (*entry_point)(void), const char* name, uint32_t stack_size, task_priority_t priority);
void delete_task(task_handle_t task);
void suspend_task(task_handle_t task);
void resume_task(task_handle_t task);
void rtos_delay(uint32_t ticks);

// Scheduler functions
void scheduler_init(void);
void scheduler_start(void);
void scheduler_tick(void);

// Mutex functions
typedef void* mutex_handle_t;
mutex_handle_t create_mutex(void);
void delete_mutex(mutex_handle_t mutex);
bool take_mutex(mutex_handle_t mutex, uint32_t timeout);
void give_mutex(mutex_handle_t mutex);

// Semaphore functions
typedef void* semaphore_handle_t;
semaphore_handle_t create_semaphore(uint32_t initial_count, uint32_t max_count);
void delete_semaphore(semaphore_handle_t semaphore);
bool take_semaphore(semaphore_handle_t semaphore, uint32_t timeout);
void give_semaphore(semaphore_handle_t semaphore);

// Queue functions
typedef void* queue_handle_t;
queue_handle_t create_queue(uint32_t item_size, uint32_t queue_size);
void delete_queue(queue_handle_t queue);
bool send_to_queue(queue_handle_t queue, const void* item, uint32_t timeout);
bool receive_from_queue(queue_handle_t queue, void* item, uint32_t timeout);

// System functions
void rtos_init(void);
void rtos_start(void);
uint32_t rtos_get_tick_count(void);
void rtos_idle_task(void); 