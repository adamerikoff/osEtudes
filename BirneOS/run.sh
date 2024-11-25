#!/bin/bash

# Define directory paths
BIN_FOLDER="./bin"
BUILD_FOLDER="./build"
SRC_FOLDER="./src"
BOOT_FOLDER="$SRC_FOLDER/boot"

ASM_FILE="$BOOT_FOLDER/boot.asm"
BIN_FILE="$BIN_FOLDER/boot.bin"

# Function to clean the bin folder
clean_bin() {
    echo "Cleaning $BIN_FOLDER..."
    if [ -d "$BIN_FOLDER" ]; then
        rm -rf "$BIN_FOLDER"/*
        echo "Cleaned $BIN_FOLDER."
    else
        echo "$BIN_FOLDER does not exist, nothing to clean."
    fi
    exit 0
}

# Ensure required directories exist
mkdir -p "$BIN_FOLDER"
mkdir -p "$BUILD_FOLDER"

# Check for clean mode
if [ "$1" == "-c" ]; then
    clean_bin
fi

# Default mode: Compile and run
echo "Compiling $ASM_FILE..."
nasm -f bin "$ASM_FILE" -o "$BIN_FILE"

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed. Please check your Assembly code for errors."
    exit 1
fi
echo "Compilation successful. Output binary: $BIN_FILE"

# Run the bootloader with QEMU
echo "Running $BIN_FILE in QEMU..."
qemu-system-x86_64 -hda "$BIN_FILE"

# Exit script
exit 0
