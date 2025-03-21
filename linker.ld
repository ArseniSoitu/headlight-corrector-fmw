ENTRY(reset_handler)

MEMORY
{
    FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 32K
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

SECTIONS
{
    .isr_vector:
    {
        . = ALIGN(4)
        KEEP(*(.isr_vector))
    }
}

PROVIDE(_stack = ORIGIN(RAM) + LENGTH(RAM));
