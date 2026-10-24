.section .text.start
.global Start
.align 4
.arm

.GwFba:   .word 0x080FFFC0
.GwFbb:   .word 0x080FFFD0
.Cakehax: .word 0x23FFFE00
.Entry:   .word 0x23F00000

Start:
    mov r9, r0
    mov r10, r1

    ldr r4, =0xBEEF
    lsl r2, #16
    lsr r2, #16
    cmp r2, r4
    movne r9, #0

    @ Disable caches / MPU
    mrc p15, 0, r4, c1, c0, 0
    bic r4, #(1<<16)
    bic r4, #(1<<12)
    bic r4, #(1<<2)
    bic r4, #(1<<0)
    mcr p15, 0, r4, c1, c0, 0

    @ Disable FIQs and IRQs
    msr cpsr_cxsf, #0xD3

    @ Disable and acknowledge interrupts
    mov r2, #0x10000000
    add r2, r2, #0x1000
    mov r0, #0
    mvn r1, #0
    strd r0, r1, [r2]

    @ Clear NDMA registers
    add r2, r2, #0x1000
    add r1, r2, #0xFC
    add r2, r2, #0x1C

DmaClearLoop:
    str r0, [r2], #0x1C
    cmp r1, r2
    bne DmaClearLoop

    @ Clear BSS
    ldr r0, =BssStart
    ldr r1, =BssEnd
    mov r2, #0

BssClr:
    cmp r0, r1
    strlt r2, [r0], #4
    blt BssClr

    @ Invalidate caches
    mov r5, #0
    mcr p15, 0, r5, c7, c5, 0
    mcr p15, 0, r5, c7, c6, 0
    mcr p15, 0, r5, c7, c10, 4

    @ Give read/write access to all memory regions
    ldr r5, =0x33333333
    mcr p15, 0, r5, c5, c0, 2
    mcr p15, 0, r5, c5, c0, 3

    @ Set MPU permissions and cache settings
    ldr r0, =0xFFFF001F
    ldr r1, =0x3000801B
    ldr r2, =0x01FF801D
    ldr r3, =0x08000029
    ldr r4, =0x10000029
    ldr r5, =0x20000037
    ldr r6, =0x1FF00027
    ldr r7, =0x1800002D
    mov r8, #0x2D

    mcr p15, 0, r0, c6, c0, 0
    mcr p15, 0, r1, c6, c1, 0
    mcr p15, 0, r2, c6, c2, 0
    mcr p15, 0, r3, c6, c3, 0
    mcr p15, 0, r4, c6, c4, 0
    mcr p15, 0, r5, c6, c5, 0
    mcr p15, 0, r6, c6, c6, 0
    mcr p15, 0, r7, c6, c7, 0
    mcr p15, 0, r8, c3, c0, 0
    mcr p15, 0, r8, c2, c0, 0
    mcr p15, 0, r8, c2, c0, 1

    @ Enable DTCM
    ldr r1, =0x3000800A
    mcr p15, 0, r1, c9, c1, 0

    @ Enable caches
    mrc p15, 0, r4, c1, c0, 0
    orr r4, r4, #(1<<18)
    orr r4, r4, #(1<<16)
    orr r4, r4, #(1<<12)
    orr r4, r4, #(1<<2)
    orr r4, r4, #(1<<0)
    mcr p15, 0, r4, c1, c0, 0

    @ Fix SDMC mounting
    ldr r0, =0x10000020
    mov r1, #0x340
    str r1, [r0]

    ldr sp, =0x23F00000

    mov r0, r9
    mov r1, r10
    b Main
