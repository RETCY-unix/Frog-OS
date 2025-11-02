OUTPUT_FORMAT("binary")
ENTRY(kernel_main)

SECTIONS
{
    . = 0x1000;
    
    .text : {
        *(.text)
    }
    
    .rodata : {
        *(.rodata)
    }
    
    .data : {
        *(.data)
    }
    
    /* Put BSS at a safe location with plenty of space */
    . = ALIGN(4096);
    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }
    
    . = ALIGN(4096);
    _kernel_end = .;
}
