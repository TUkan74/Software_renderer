# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(3D_Software_Renderer VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define paths for source and include directories
set(SRC_DIR src)
set(INCLUDE_DIR include)

# Collect all source files in the src directory
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")

# Define the executable target
add_executable(renderer ${SRC_FILES})

# Include the headers
target_include_directories(renderer PRIVATE ${INCLUDE_DIR})

# Set output directories for build artifacts
set_target_properties(renderer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)

# Add a custom clean target to remove all build artifacts
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/build/renderer.exe
    COMMENT "Cleaning up all build artifacts, including executables"
)


# Platform-specific settings
if(WIN32)
    message(STATUS "Compiling for Windows")
    # Add any Windows-specific flags or libraries here
elseif(UNIX)
    message(STATUS "Compiling for Linux/Unix")
    # Add any Linux-specific flags or libraries here
endif()

# Enable verbose build commands (optional)
set(CMAKE_VERBOSE_MAKEFILE ON)
