ENTRY(_start)

OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 1M; /* Set the start address to 1MB */

    .text : ALIGN(4096)
    {
        *(.text) /* Include all sections named .text */
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata) /* Include all sections named .rodata */
    }

    .data : ALIGN(4096)
    {
        *(.data) /* Include all sections named .data */
    }

    .bss : ALIGN(4096)
    {
        *(COMMON) /* Include common symbols (used for uninitialized variables) */
        *(.bss)   /* Include all sections named .bss (uninitialized data) */
    }

    .asm : ALIGN(4096)
    {
        *(.asm)
    }
}
