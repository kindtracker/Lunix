OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Start)

SECTIONS
{
    . = 0x08006800;

    .text.start : ALIGN(4) { *(.text.start) }
    .text       : ALIGN(4) { *(.text*) }
    .rodata     : ALIGN(4) { *(.rodata*) }
    .data       : ALIGN(4) { *(.data*) }
    .bss        : ALIGN(4)
    {
        __bss_start = .;
        *(.bss* COMMON)
        __bss_end = .;
    }

    . = ALIGN(4096);

    MemoryStart = 0x20006800;
    MemoryEnd = 0x28000000;

    __end__ = ABSOLUTE(.);
}
