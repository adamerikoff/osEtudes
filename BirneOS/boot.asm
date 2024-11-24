ORG 0x7c00           ; Origin: the memory address where the bootloader will be loaded.
BITS 16              ; Specify 16-bit mode (real mode).

start:
    mov ah, 0eh       ; BIOS Teletype output function (INT 0x10, AH=0x0E).
    mov al, 'A'       ; Load the character 'A' into the AL register.
    int 0x10          ; Call BIOS interrupt to print the character.

    jmp $             ; Infinite loop to halt execution.

; Fill remaining space to 512 bytes with zeros
times 510-($ - $$) db 0

; Boot signature
dw 0xAA55            ; Bootable disk signature.
