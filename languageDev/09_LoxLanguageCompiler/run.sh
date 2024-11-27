#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Define variables
BUILD_DIR="build"
EXECUTABLE="loxCompiler"

# Step 1: Create a build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir $BUILD_DIR
fi

# Step 2: Navigate to the build directory
cd $BUILD_DIR

# Step 3: Run CMake to generate the build system
echo "Generating build system with CMake..."
cmake ..

# Step 4: Build the project
echo "Building the project..."
cmake --build .

# Step 5: Run the executable
echo "Running the program..."
./bin/$EXECUTABLE
