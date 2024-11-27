# Simple Bootloader

This project demonstrates a minimal bootloader written in Assembly. When executed, it prints the letter `A` to the screen using BIOS interrupt `0x10`.

## Contents

- `boot.asm`: The Assembly source code for the bootloader.

## How to Build and Run

### Prerequisites

1. **NASM**: The Netwide Assembler (to assemble the bootloader).
2. **QEMU**: A lightweight emulator to test the bootloader.

### Steps to Run

1. **Assemble the Code**
   Compile the Assembly source code into a binary file using NASM:
   ```bash
   nasm ./boot.asm -f bin -o ./boot.bin
   ```

2. **Run with QEMU**
   Test the bootloader using QEMU:
   ```bash
   qemu-system-x86_64 -hda ./boot.bin
   ```

### Expected Output

When you run the bootloader, you should see the letter **`A`** printed on the screen.