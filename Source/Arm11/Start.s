.section .text.start
.global Start11
.align 4
.arm

Start11:
    cpsid if
    ldr sp, =_Stack11Top
