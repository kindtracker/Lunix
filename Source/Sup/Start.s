.arm
.syntax unified
.fpu vfp

/* ARM processor modes */
.equ MODE_USR, 0x10
.equ MODE_ABT, 0x17
.equ MODE_UND, 0x1B
.equ MODE_SVC, 0x13
.equ MODE_IRQ, 0x12

/* Offsets into the job context structure */
.equ CTX_PC,    60     /* Saved program counter */
.equ CTX_CPSR,  64     /* Saved processor status register */
.equ CTX_FPSCR, 68     /* Floating-point status register */
.equ CTX_D,     72     /* Floating-point D registers */
.equ CTX_TLS,   200    /* Thread-local storage */

/* Entry point for starting a job */
.global job_start
.type job_start, %function

job_start:
    cpsid   if                  /* Disable IRQ and FIQ interrupts */

    push    {r4-r12, lr}        /* Preserve callee-saved registers and return address */

    ldr     r1, =host_sp        /* Load address of host stack pointer storage */
    str     sp, [r1]            /* Save the host's current stack pointer */

    mrs     r2, cpsr            /* Read the current processor status register */
    ldr     r1, =host_cpsr      /* Load address of host CPSR storage */
    str     r2, [r1]            /* Save the host's CPSR */

    mov     r4, r0              /* Preserve job argument across function calls */
    ldr     r5, =host_banked    /* Load address of banked register storage */

    cps     #MODE_SVC           /* Switch to Supervisor mode */
    stmia   r5!, {sp, lr}       /* Save SVC stack pointer and link register */

    cps     #MODE_ABT           /* Switch to Abort mode */
    stmia   r5!, {sp, lr}       /* Save Abort stack pointer and link register */
    ldr     sp, =fault_frame    /* Set Abort mode stack pointer */

    cps     #MODE_UND           /* Switch to Undefined mode */
    stmia   r5!, {sp, lr}       /* Save Undefined stack pointer and link register */
    ldr     sp, =fault_frame    /* Set Undefined mode stack pointer */

    cps     #MODE_IRQ           /* Switch to IRQ mode */
    stmia   r5!, {sp, lr}       /* Save IRQ stack pointer and link register */
    ldr     sp, =irq_scratch    /* Set IRQ mode stack pointer */

    cps     #MODE_SVC           /* Return to Supervisor mode */
    ldr     sp, =sup_stack_top  /* Switch to the supervisor's stack */

    mov     r0, r4              /* Restore job argument into first argument register */
    bl      mmu_enter           /* Enter the job's MMU context */

    mov     r0, r4              /* Pass job argument to job_main */
    bl      job_main            /* Execute the job */

    b       sup_yield           /* Return control to the supervisor */

/* Entry point for yielding from a job back to the supervisor */
.global sup_yield
.type sup_yield, %function

sup_yield:
    /* Context restoration needs to be implemented here */
    b       sup_yield

.global v_other
.global v_undef
.global v_svc
.global v_pabt
.global v_dabt
.global v_irq

v_other:
    b v_other

v_undef:
    b v_undef

v_svc:
    b v_svc

v_pabt:
    b v_pabt

v_dabt:
    b v_dabt

v_irq:
    b v_irq

.global vector_table
.global vector_table_end
.balign 4                  /* Align vector table to a 4-byte boundary */
vector_table:
    ldr     pc, [pc, #0x18]    /* Reset vector: load handler address into PC */
    ldr     pc, [pc, #0x18]    /* Undefined instruction exception */
    ldr     pc, [pc, #0x18]    /* Supervisor Call (SVC) exception */
    ldr     pc, [pc, #0x18]    /* Prefetch Abort exception */
    ldr     pc, [pc, #0x18]    /* Data Abort exception */
    ldr     pc, [pc, #0x18]    /* Reserved vector */
    ldr     pc, [pc, #0x18]    /* IRQ interrupt */
    ldr     pc, [pc, #0x18]    /* FIQ interrupt */

    /* Handler addresses, in the same order as the vector instructions */
    .word   v_other             /* Reset handler */
    .word   v_undef             /* Undefined instruction handler */
    .word   v_svc               /* SVC handler */
    .word   v_pabt              /* Prefetch Abort handler */
    .word   v_dabt              /* Data Abort handler */
    .word   v_other             /* Reserved vector handler */
    .word   v_irq               /* IRQ handler */
    .word   v_other             /* FIQ handler */

vector_table_end:               /* Address immediately after the vector table */

.ltorg                  /* Emit pending literal pool here */

.balign 4

host_sp:     .space 4
host_cpsr:   .space 4
host_banked: .space 32
irq_scratch: .space 8

.global sup_current_ctx
sup_current_ctx: .space 4

.balign 8
.global fault_frame
fault_frame: .space 256

.balign 8
sup_stack:   .space 0x8000

.global sup_stack_top
sup_stack_top:
