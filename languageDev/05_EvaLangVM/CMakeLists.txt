cmake_minimum_required(VERSION 3.31)
project(EvaLangVM VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure correct macOS SDK (remove this line if not needed on other OSes)
if(APPLE)
    set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk)
endif()

# Define source directories
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(VM_DIR ${SRC_DIR}/vm)
set(BC_DIR ${SRC_DIR}/bytecode)
set(PR_DIR ${SRC_DIR}/parser)
set(CM_DIR ${SRC_DIR}/compiler)
set(DS_DIR ${SRC_DIR}/disassembler)
set(LOG_DIR ${SRC_DIR}/logger)

# Gather source files
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${CM_DIR}/EvaCompiler.cpp
    ${CM_DIR}/Scope.cpp
    ${DS_DIR}/EvaDisassembler.cpp
    ${VM_DIR}/EvaVM.cpp
    ${VM_DIR}/EvaCollector.cpp
    ${VM_DIR}/Global.cpp
    ${BC_DIR}/OpCode.h
    ${LOG_DIR}/Logger.h
    ${PR_DIR}/EvaParser.h
)

# Include directories
include_directories(
    ${SRC_DIR}
    ${VM_DIR}
    ${BC_DIR}
    ${PR_DIR}
    ${CM_DIR}
    ${DS_DIR}
    ${LOG_DIR}
)

# Add executable target
add_executable(EvaLangVM ${SOURCES})

# Link any necessary libraries (e.g., pthreads on Linux)
if(UNIX AND NOT APPLE)
    target_link_libraries(EvaLangVM pthread)
endif()

# Set compiler options
target_compile_options(EvaLangVM PRIVATE)

# Organize source files in IDEs
source_group(TREE ${SRC_DIR} PREFIX "Source Files" FILES ${SOURCES})

# Install targets
install(TARGETS EvaLangVM RUNTIME DESTINATION bin)
