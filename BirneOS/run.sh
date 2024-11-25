#!/bin/bash

# Define directory paths
BIN_FOLDER="./bin"
BUILD_FOLDER="./build"
SRC_FOLDER="./src"

BOOT_FOLDER="$SRC_FOLDER/boot"

ASM_FILE="$BOOT_FOLDER/boot.asm"
BIN_FILE="$BIN_FOLDER/boot.bin"

# Ensure required directories exist
mkdir -p "$BIN_FOLDER"
mkdir -p "$BUILD_FOLDER"

# Step 1: Assemble the code
echo "Compiling $ASM_FILE..."
nasm -f bin "$ASM_FILE" -o "$BIN_FILE"

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check your Assembly code for errors."
    exit 1
fi
echo "Compilation successful. Output binary: $BIN_FILE"

# Step 2: Run the bootloader with QEMU
echo "Running $BIN_FILE in QEMU..."
qemu-system-x86_64 -hda "$BIN_FILE"

# Exit script
exit 0
