#include "rtos.h"
#include <string.h>

// Maximum number of tasks
#define MAX_TASKS 16

// Task control blocks
static task_control_block_t task_list[MAX_TASKS];
static uint32_t num_tasks = 0;
static task_control_block_t* current_task = NULL;
static uint32_t tick_count = 0;

// Task stack initialization
static void init_task_stack(task_control_block_t* task) {
    // Initialize stack with pattern
    for (uint32_t i = 0; i < task->stack_size; i++) {
        task->stack[i] = 0xDEADBEEF;
    }
    
    // Set up initial stack frame
    uint32_t* sp = &task->stack[task->stack_size - 1];
    
    // Simulate context saved by hardware
    *(--sp) = 0x00000000;  // R0
    *(--sp) = 0x00000000;  // R1
    *(--sp) = 0x00000000;  // R2
    *(--sp) = 0x00000000;  // R3
    *(--sp) = 0x00000000;  // R4
    *(--sp) = 0x00000000;  // R5
    *(--sp) = 0x00000000;  // R6
    *(--sp) = 0x00000000;  // R7
    *(--sp) = 0x00000000;  // R8
    *(--sp) = 0x00000000;  // R9
    *(--sp) = 0x00000000;  // R10
    *(--sp) = 0x00000000;  // R11
    *(--sp) = 0x00000000;  // R12
    *(--sp) = 0x00000000;  // LR
    *(--sp) = 0x00000000;  // PC
    *(--sp) = 0x00000013;  // CPSR (SVC mode)
    
    task->stack_pointer = (uint32_t)sp;
}

// Task creation
task_handle_t create_task(void (*entry_point)(void), const char* name, uint32_t stack_size, task_priority_t priority) {
    if (num_tasks >= MAX_TASKS) {
        return NULL;
    }
    
    task_control_block_t* task = &task_list[num_tasks++];
    
    // Initialize task control block
    strncpy(task->name, name, sizeof(task->name) - 1);
    task->name[sizeof(task->name) - 1] = '\0';
    task->entry_point = entry_point;
    task->stack_size = stack_size;
    task->priority = priority;
    task->state = TASK_READY;
    task->sleep_ticks = 0;
    
    // Allocate stack
    task->stack = (uint32_t*)malloc(stack_size * sizeof(uint32_t));
    if (!task->stack) {
        num_tasks--;
        return NULL;
    }
    
    // Initialize stack
    init_task_stack(task);
    
    return task;
}

// Task deletion
void delete_task(task_handle_t task) {
    task_control_block_t* tcb = (task_control_block_t*)task;
    
    // Free stack
    free(tcb->stack);
    
    // Remove from task list
    for (uint32_t i = 0; i < num_tasks; i++) {
        if (&task_list[i] == tcb) {
            // Shift remaining tasks
            for (uint32_t j = i; j < num_tasks - 1; j++) {
                task_list[j] = task_list[j + 1];
            }
            num_tasks--;
            break;
        }
    }
}

// Task suspension
void suspend_task(task_handle_t task) {
    task_control_block_t* tcb = (task_control_block_t*)task;
    tcb->state = TASK_SUSPENDED;
}

// Task resumption
void resume_task(task_handle_t task) {
    task_control_block_t* tcb = (task_control_block_t*)task;
    if (tcb->state == TASK_SUSPENDED) {
        tcb->state = TASK_READY;
    }
}

// Task delay
void rtos_delay(uint32_t ticks) {
    if (current_task) {
        current_task->sleep_ticks = ticks;
        current_task->state = TASK_BLOCKED;
    }
}

// Scheduler initialization
void scheduler_init(void) {
    num_tasks = 0;
    current_task = NULL;
    tick_count = 0;
}

// Scheduler tick
void scheduler_tick(void) {
    tick_count++;
    
    // Update task states
    for (uint32_t i = 0; i < num_tasks; i++) {
        task_control_block_t* task = &task_list[i];
        
        if (task->state == TASK_BLOCKED) {
            if (task->sleep_ticks > 0) {
                task->sleep_ticks--;
                if (task->sleep_ticks == 0) {
                    task->state = TASK_READY;
                }
            }
        }
    }
    
    // Find highest priority ready task
    task_control_block_t* next_task = NULL;
    task_priority_t highest_priority = 0;
    
    for (uint32_t i = 0; i < num_tasks; i++) {
        task_control_block_t* task = &task_list[i];
        
        if (task->state == TASK_READY && task->priority > highest_priority) {
            next_task = task;
            highest_priority = task->priority;
        }
    }
    
    // Perform context switch if needed
    if (next_task && next_task != current_task) {
        if (current_task) {
            current_task->state = TASK_READY;
        }
        current_task = next_task;
        current_task->state = TASK_RUNNING;
        
        // Context switch would happen here
        // In a real implementation, this would save the current context
        // and restore the new task's context
    }
}

// RTOS initialization
void rtos_init(void) {
    scheduler_init();
}

// RTOS start
void rtos_start(void) {
    // Start the first task
    if (num_tasks > 0) {
        current_task = &task_list[0];
        current_task->state = TASK_RUNNING;
        
        // Start the task
        // In a real implementation, this would set up the initial context
        // and start the first task
    }
}

// Get tick count
uint32_t rtos_get_tick_count(void) {
    return tick_count;
}

// Idle task
void rtos_idle_task(void) {
    while (1) {
        // Enter low-power mode
        // In a real implementation, this would put the CPU in a low-power state
    }
} 