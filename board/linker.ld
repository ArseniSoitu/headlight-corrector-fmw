ENTRY(Reset_Handler)

MEMORY
{
    FLASH(rx) : ORIGIN = 0x08000000, LENGTH = 32K
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 6K
}

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7) /* glue arm to thumb code */
        *(.glue_7t) /* glue thumb to arm code */
        *(.eh_frame) /* call frame information records, need for C++ */

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .; /* end of code section */
    } >FLASH

    /* Constant data */
    .rodata :
    {
        . = ALIGN(4);
        *(.rodata) /* constants, const strings, etc */
        *(.rodata*) /* constants, const strings, etc */
        . = ALIGN(4);
    } >FLASH

    /* Need for newlib with 64bit printf suppoert for C++ */
    .ARM.extab :
    {
        *(.ARM.extab*)
        *(.gnu.linkonce.armextab.*)
    } >FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH

    /* C++ static constructors/destructors */
    .preinit_array :
    {
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
    } >FLASH

    .init_array :
    {
        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*))) /* SORT is an alias for SORT_BY_NAME */
        KEEP (*(.init_array))
        __init_array_end = .;
    } >FLASH

    .fini_array :
    {
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array))
        __fini_array_end = .;
    } >FLASH

    /* ram, but not cleared on reset for ex: global variables */
    .noinit (NOLOAD) :
    {
        *(.noinit*)
    } >RAM
    . = ALIGN(4);

    /* initialized data, load LMA copy after code */
    .data :
    {
        _sdata = .;
        *(.data*)
        *(.data)
        *(.ramtext*)
        _edata = .;
    } >RAM AT >FLASH

    /* used by the startup code to initialize data */
    _data_loadaddr = LOADADDR(.data);

    /* uninitialized data */
    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 :
    {
        *(.ARM.attributes)
    }
}

PROVIDE(_stack = ORIGIN(RAM) + LENGTH(RAM)); /* PROVIDE IS SIMILAR TO PROVIDE_HIDDEN */
