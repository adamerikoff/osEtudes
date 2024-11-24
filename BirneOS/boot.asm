ORG 0x7c00           ; The memory address where the BIOS loads the bootloader.
BITS 16              ; Specifies that the code is for 16-bit mode (real mode).

start:
    mov si, message   ; Load the address of the 'message' string into the SI register (source index).
    call print        ; Call the 'print' subroutine to display the message on the screen.
    jmp $             ; Infinite loop to halt execution after printing the message.
                      ; '$' refers to the current address, so this creates a loop.

; Subroutine to print a null-terminated string from memory.
print:
    mov bx, 0         ; Clear the BX register (not strictly necessary in this code).
.loop:
    lodsb             ; Load the byte at [SI] (pointed by SI) into AL, and increment SI.
    cmp al, 0         ; Compare the loaded byte in AL with 0 (null terminator).
    je .done          ; If AL == 0 (end of the string), jump to the 'done' label to exit the loop.
    call print_char   ; Otherwise, call the 'print_char' subroutine to print the character in AL.
    jmp .loop         ; Repeat the loop for the next character in the string.
.done:
    ret               ; Return from the 'print' subroutine.

; Subroutine to print a single character using BIOS interrupt 0x10.
print_char:
    mov ah, 0eh       ; BIOS teletype output function (AH=0x0E) to display a character on the screen.
    int 0x10          ; Call BIOS interrupt 0x10 to print the character in AL.
    ret               ; Return from the 'print_char' subroutine.

; Null-terminated message string to display on the screen.
message: db 'Hello World', 0   ; Define the string "Hello World" followed by a null byte (0).

; Fill the remaining space in the boot sector with zeros.
; The boot sector must be exactly 512 bytes.
times 510-($ - $$) db 0        ; Pad with zeros from the current position ($) to the 510th byte.

; Boot sector signature required by the BIOS to identify the disk as bootable.
dw 0xAA55                     ; The last two bytes of the boot sector, the mandatory boot signature.
