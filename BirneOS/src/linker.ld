ENTRY(_start)

OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 1M; /* Set the start address to 1MB */

    .text :
    {
        *(.text) /* Include all sections named .text */
    }

    .rodata :
    {
        *(.rodata) /* Include all sections named .rodata */
    }

    .data :
    {
        *(.data) /* Include all sections named .data */
    }

    .bss :
    {
        *(COMMON) /* Include common symbols (used for uninitialized variables) */
        *(.bss)   /* Include all sections named .bss (uninitialized data) */
    }
}
