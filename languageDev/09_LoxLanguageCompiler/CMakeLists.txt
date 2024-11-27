cmake_minimum_required(VERSION 3.31.0)

# Set the project name and version
project(loxCompiler VERSION 1.0 LANGUAGES C)

# Specify the C standard to use
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Specify output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories (use src/headers as the include path)
include_directories(${PROJECT_SOURCE_DIR}/src/headers)

# Collect all source files
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link necessary libraries (if any)
target_link_libraries(${PROJECT_NAME} PRIVATE m)

# Add compiler warnings and optimizations
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
