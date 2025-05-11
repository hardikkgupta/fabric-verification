.section .text

.global enable_interrupts
.global disable_interrupts
.global save_context
.global restore_context

// Enable interrupts
enable_interrupts:
    mrs r0, cpsr
    bic r0, r0, #0x80    // Clear I bit
    msr cpsr_c, r0
    bx lr

// Disable interrupts
disable_interrupts:
    mrs r0, cpsr
    orr r0, r0, #0x80    // Set I bit
    msr cpsr_c, r0
    bx lr

// Save context (registers and status)
save_context:
    // Save registers r0-r12, lr
    stmfd sp!, {r0-r12, lr}
    
    // Save CPSR
    mrs r0, cpsr
    stmfd sp!, {r0}
    
    // Save SPSR if in exception mode
    mrs r0, spsr
    stmfd sp!, {r0}
    
    bx lr

// Restore context
restore_context:
    // Restore SPSR
    ldmfd sp!, {r0}
    msr spsr_cxsf, r0
    
    // Restore CPSR
    ldmfd sp!, {r0}
    msr cpsr_cxsf, r0
    
    // Restore registers r0-r12, lr
    ldmfd sp!, {r0-r12, lr}
    
    bx lr

// Exception handlers
.section .text.exceptions

// IRQ handler
irq_handler:
    // Save context
    bl save_context
    
    // Handle IRQ
    bl handle_irq
    
    // Restore context
    bl restore_context
    
    // Return from exception
    subs pc, lr, #4

// FIQ handler
fiq_handler:
    // Save context
    bl save_context
    
    // Handle FIQ
    bl handle_fiq
    
    // Restore context
    bl restore_context
    
    // Return from exception
    subs pc, lr, #4

// Undefined instruction handler
undefined_handler:
    // Save context
    bl save_context
    
    // Handle undefined instruction
    bl handle_undefined
    
    // Restore context
    bl restore_context
    
    // Return from exception
    subs pc, lr, #4

// Data abort handler
data_abort_handler:
    // Save context
    bl save_context
    
    // Handle data abort
    bl handle_data_abort
    
    // Restore context
    bl restore_context
    
    // Return from exception
    subs pc, lr, #8

// Prefetch abort handler
prefetch_abort_handler:
    // Save context
    bl save_context
    
    // Handle prefetch abort
    bl handle_prefetch_abort
    
    // Restore context
    bl restore_context
    
    // Return from exception
    subs pc, lr, #4

// Software interrupt handler
swi_handler:
    // Save context
    bl save_context
    
    // Handle software interrupt
    bl handle_swi
    
    // Restore context
    bl restore_context
    
    // Return from exception
    movs pc, lr

// Exception vector table
.section .text.vectors
    ldr pc, reset_handler_addr
    ldr pc, undefined_handler_addr
    ldr pc, swi_handler_addr
    ldr pc, prefetch_abort_handler_addr
    ldr pc, data_abort_handler_addr
    ldr pc, unused_handler_addr
    ldr pc, irq_handler_addr
    ldr pc, fiq_handler_addr

// Exception handler addresses
reset_handler_addr:        .word reset_handler
undefined_handler_addr:    .word undefined_handler
swi_handler_addr:         .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:   .word data_abort_handler
unused_handler_addr:      .word unused_handler
irq_handler_addr:         .word irq_handler
fiq_handler_addr:         .word fiq_handler 