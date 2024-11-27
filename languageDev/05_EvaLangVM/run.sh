#!/bin/bash

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

# Navigate to the build directory
cd build

# Run CMake to configure the project
cmake ..

# Build the project
make

# Check if the executable was created successfully
if [ -f "EvaLangVM" ]; then
    echo "Running EvaVM..."
    ./EvaLangVM
else
    echo "Build failed: EvaVM executable not found."
fi
