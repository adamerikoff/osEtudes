#!/bin/bash

# Create the build directory if it doesn't exist
if [ ! -d "build" ]; then
  mkdir build
fi

# Navigate to the build directory
cd build

# Run CMake to configure the build
cmake ..

# Compile the project
make

# Run the executable if the build was successful
if [ $? -eq 0 ]; then
  ./PhotonForge
else
  echo "Build failed."
fi
