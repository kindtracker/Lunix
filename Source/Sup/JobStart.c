#include <stdint.h>

/* ARM processor modes */
#define MODE_USR 0x10
#define MODE_FIQ 0x11
#define MODE_IRQ 0x12
#define MODE_SVC 0x13
#define MODE_ABT 0x17
#define MODE_UND 0x1B

/* Host context */
__attribute__((used, aligned(4))) uint32_t host_sp;

__attribute__((used, aligned(4))) uint32_t host_cpsr;

__attribute__((used, aligned(4))) uint32_t host_banked[8];

/* Scratch stacks */
__attribute__((used, aligned(8))) uint32_t irq_scratch[2];

__attribute__((used, aligned(8))) uint32_t fault_frame[64];

__attribute__((used, aligned(8))) uint8_t sup_stack[0x8000];

__attribute__((used)) uint32_t sup_current_ctx;

/* Stack top address, calculated in C */
#define SUP_STACK_TOP ((uintptr_t)(sup_stack + sizeof(sup_stack)))

extern void mmu_enter(void);
extern void job_main(void);
extern void sup_yield(void);

/*
 * Start a job.
 *
 * Argument:
 *   Job context pointer.
 */
__attribute__((naked, used, section(".text.entry"))) void job_start(void) {
  __asm__ volatile("cpsid if\n"

                   /* Save host registers */
                   "push {r4-r12, lr}\n"

                   "ldr r1, =host_sp\n"
                   "str sp, [r1]\n"

                   "mrs r2, cpsr\n"
                   "ldr r1, =host_cpsr\n"
                   "str r2, [r1]\n"

                   "mov r4, r0\n"
                   "ldr r5, =host_banked\n"

                   /* Save SVC banked registers */
                   "cps #0x13\n"
                   "stmia r5!, {sp, lr}\n"

                   /* Save ABT banked registers */
                   "cps #0x17\n"
                   "stmia r5!, {sp, lr}\n"
                   "ldr sp, =fault_frame\n"

                   /* Save UND banked registers */
                   "cps #0x1B\n"
                   "stmia r5!, {sp, lr}\n"
                   "ldr sp, =fault_frame\n"

                   /* Save IRQ banked registers */
                   "cps #0x12\n"
                   "stmia r5!, {sp, lr}\n"
                   "ldr sp, =irq_scratch\n"

                   /* Switch back to supervisor mode */
                   "cps #0x13\n"

                   /* Set supervisor stack */
                   "ldr sp, =sup_stack\n"
                   "add sp, sp, #0x8000\n"

                   /* Enter job */
                   "mov r0, r4\n"
                   "bl mmu_enter\n"

                   "mov r0, r4\n"
                   "bl job_main\n"

                   "b sup_yield\n");
}

/*
 * Yield execution.
 */
__attribute__((naked, used)) void sup_yield(void) {
  __asm__ volatile("1:\n"
                   "b 1b\n");
}

/* Exception handlers */

__attribute__((interrupt("IRQ"), used)) void v_irq(void) {
  for (;;) {
  }
}

__attribute__((interrupt("ABORT"), used)) void v_pabt(void) {
  for (;;) {
  }
}

__attribute__((interrupt("ABORT"), used)) void v_dabt(void) {
  for (;;) {
  }
}

__attribute__((interrupt("UNDEF"), used)) void v_undef(void) {
  for (;;) {
  }
}

__attribute__((interrupt("SWI"), used)) void v_svc(void) {
  for (;;) {
  }
}

/* Default handler */
__attribute__((used)) void v_other(void) {
  for (;;) {
  }
}

/*
 * ARM vector table.
 *
 * This is deliberately kept in assembly because the CPU
 * expects instructions at fixed vector offsets.
 */
__asm__(".section .vectors, \"ax\", %progbits\n"
        ".balign 32\n"

        ".global vector_table\n"
        ".global vector_table_end\n"

        "vector_table:\n"

        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"
        "ldr pc, [pc, #0x18]\n"

        ".word v_other\n"
        ".word v_undef\n"
        ".word v_svc\n"
        ".word v_pabt\n"
        ".word v_dabt\n"
        ".word v_other\n"
        ".word v_irq\n"
        ".word v_other\n"

        "vector_table_end:\n");

/*
 * Stack-top helper if C code needs the address.
 */
uintptr_t sup_stack_top_address(void) { return SUP_STACK_TOP; }
