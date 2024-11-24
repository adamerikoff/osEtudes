ORG 0
BITS 16
_start:
    jmp short start
    nop

times 33 db 0

start:
    jmp 0x7c0:step2

handle_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret

handle_one:
    mov ah, 0eh
    mov al, 'V'
    mov bx, 0x00
    int 0x10
    iret


step2:
    cli
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti
    mov word[ss:0x00], handle_zero
    mov word[ss:0x02], 0x7c0
    mov word[ss:0x04], handle_one
    mov word[ss:0x06], 0x7c0
    int 1
    mov si, message
    call print
    jmp $

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
message: db 'Hello World!', 0   ; Define the string "Hello World" followed by a null byte (0).

; Fill the remaining space in the boot sector with zeros.
; The boot sector must be exactly 512 bytes.
times 510-($ - $$) db 0        ; Pad with zeros from the current position ($) to the 510th byte.

; Boot sector signature required by the BIOS to identify the disk as bootable.
dw 0xAA55                     ; The last two bytes of the boot sector, the mandatory boot signature.
