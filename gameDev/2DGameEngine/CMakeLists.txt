# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.30.5)

# Define the project name and specify that it's a C++ project
project(2DGameEngine LANGUAGES CXX)

# Set the C++ standard to C++23 (or the version you're using)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Include directories for engine and game modules
include_directories(src/engine)

# Add the executable by specifying the output name and the source files
add_executable(
    2DGameEngine 
    src/main.cpp
    src/engine/Game.cpp
)

target_link_libraries(2DGameEngine ${SDL2_LIBRARIES})

# Optional: Ensure SDL2 is linked correctly
if (SDL2_FOUND)
    message(STATUS "SDL2 found and linked successfully.")
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()
