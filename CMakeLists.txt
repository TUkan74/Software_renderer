cmake_minimum_required(VERSION 3.10)
project(SoftwareRenderer)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Source files
file(GLOB SRC_FILES src/*.cpp)

# Add executable
add_executable(software-renderer ${SRC_FILES})

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.11.0 # Use the latest stable version or specify one
)

FetchContent_MakeAvailable(spdlog)

target_link_libraries(software-renderer PRIVATE spdlog::spdlog)


# Eigen library
include(FetchContent)
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0 # Specify the desired Eigen version
)
FetchContent_MakeAvailable(Eigen)
target_include_directories(software-renderer PRIVATE ${eigen_SOURCE_DIR})
