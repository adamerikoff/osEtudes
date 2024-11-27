# Specify the minimum version of CMake
cmake_minimum_required(VERSION 3.30.5)

# Define the project name and specify that it's a C project
project(PhotonForge LANGUAGES C)

# Set the C standard to C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Include directories for engine and game modules
include_directories(src/engine)
include_directories(src/game)

# Add the executable by specifying the output name and the source files
add_executable(
    PhotonForge 
    src/main.c
    src/engine/engine.c
    src/engine/map/map.c
    src/engine/player/player.c
    src/engine/ray/ray.c
)

target_link_libraries(PhotonForge ${SDL2_LIBRARIES})

