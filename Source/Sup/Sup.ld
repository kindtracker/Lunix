OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(job_start)

SECTIONS
{
    . = 0x25100000;

    .text :
    {
        KEEP(*(.text.entry))
        *(.text .text.*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
    }

    .data :
    {
        *(.data .data.*)
        . = ALIGN(4);
    }

    .bss :
    {
        *(.bss .bss.*)
        *(COMMON)
    }

    .vectors :
    {
        KEEP(*(.vectors))
    }

    ASSERT(. <= 0x25200000, "the ARM11 job runs into its parameter block")

    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.extab*)
        *(.ARM.attributes*)
        *(.comment*)
        *(.note*)
    }
}
