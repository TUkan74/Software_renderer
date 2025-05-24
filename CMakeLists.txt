cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer LANGUAGES C CXX)  # Specify only C and C++ languages

# Disable Fortran explicitly
set(CMAKE_DISABLE_FIND_PACKAGE_Fortran TRUE)
set(CMAKE_Fortran_COMPILER "")

set(CMAKE_CXX_STANDARD 20)

# Create directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/logs)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples)

# Include directories
include_directories(include)

# Source files - gather all cpp files in src directory
file(GLOB SRC_FILES 
    src/*.cpp
    src/TGATextureLoader.cpp
)

# Add executable
add_executable(software-renderer ${SRC_FILES})

include(FetchContent)

# Fetch spdlog library
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0 # Use the latest stable version or specify one
)
FetchContent_MakeAvailable(spdlog)
target_link_libraries(software-renderer PRIVATE spdlog::spdlog)

# Fetch Eigen library
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0 # Specify the desired Eigen version
)
FetchContent_MakeAvailable(Eigen)
target_include_directories(software-renderer PRIVATE ${eigen_SOURCE_DIR})

# Define a custom target to generate test textures
add_custom_target(
    generate_test_textures
    COMMAND software-renderer --generate-test-textures
    DEPENDS software-renderer
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating test textures"
)
