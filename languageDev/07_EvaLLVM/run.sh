#!/bin/bash

# Buildir
BUILD_DIR="build"

# Path to LLVM Config executable and tools
LLVM_CONFIG="/opt/homebrew/Cellar/llvm/19.1.3/bin/llvm-config"
LLVM_LLVM_DIR="/opt/homebrew/Cellar/llvm/19.1.3/bin"
LLVM_LLVM_LLI="$LLVM_LLVM_DIR/lli"

# Output filenames
EXEC="$BUILD_DIR/eva-llvm"
LLIR_BYTECODE="$BUILD_DIR/out.ll"

# Create the build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Compile the main.cpp file to a native executable
clang++ -o "$EXEC" \
    -target arm64-apple-darwin \
    $("$LLVM_CONFIG" --cxxflags --ldflags --system-libs --libs all) \
    main.cpp

# Generate LLVM IR from the C++ source
clang++ -S -emit-llvm -o "$LLIR_BYTECODE" \
    $("$LLVM_CONFIG" --cxxflags) \
    main.cpp

# Function to show usage
usage() {
    echo "Usage: $0 [-e | -i]"
    echo "  -e  Run the compiled executable."
    echo "  -i  Run the LLVM IR using lli (default)."
    exit 1
}

# Parse the flag argument
if [ $# -eq 0 ]; then
    # If no flag is provided, default to running LLVM IR using lli
    echo "No flag provided. Running the LLVM IR using lli (default)..."
    "$LLVM_LLVM_LLI" "$LLIR_BYTECODE"
    exit 0
fi

while getopts "ei" opt; do
    case "$opt" in
        e)
            # Run the compiled executable
            echo "Running the compiled executable ($EXEC)..."
            "$EXEC"
            ;;
        i)
            # Run the LLVM IR using lli
            echo "Running the LLVM IR using lli ($LLIR_BYTECODE)..."
            "$LLVM_LLVM_LLI" "$LLIR_BYTECODE"
            echo $?
            printf "\n"
            ;;
        *)
            usage
            ;;
    esac
done