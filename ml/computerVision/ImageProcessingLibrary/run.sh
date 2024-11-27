#!/bin/bash

# Variables for source files and output
main="main.cpp"
imageProcessing="./lib/ImageProcessing.cpp"
output="output.o"
cpp_standard="c++17"  # Set your desired C++ standard

# Function to clean up the generated binary
cleanup() {
    echo "Cleaning up..."
    rm -f "$output"
    echo "Compiled binary was removed."
}

# Compile main.cpp and ImageProcessing.cpp
if g++ -std="$cpp_standard" "$main" "$imageProcessing" -o "$output"; then
    echo "Compilation successful."
    ./"$output"
else
    echo "Compilation failed."
    exit 1
fi

# Clean up after execution
trap cleanup EXIT
