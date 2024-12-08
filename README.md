# Software Renderer

This project implements a basic **software renderer** in C++ from scratch. It focuses on rendering 3D models in the OBJ format without relying on external graphics libraries like OpenGL or DirectX. The software renderer is educational, offering features such as wireframe rendering, colored polygons, and texture mapping with TGA files.

## Features
- **OBJ File Parsing**: Parse and render 3D models stored in Wavefront OBJ format.
- **Basic Rendering**: Render models using wireframe, solid color, or texture mapping techniques.
- **Matrix Operations**: Utilize Eigen library for matrix and vector computations.
- **Logging**: Use spdlog for detailed logging of the rendering process.

## Project Structure
```
software-renderer/
├── build/                 # Directory for build artifacts
├── include/               # Header files
├── src/                   # Source files
│   ├── main.cpp           # Entry point of the application
├── logs/                  # Directory for log files
├── tests/                 # Unit tests
├── CMakeLists.txt         # CMake configuration file
├── README.md              # Project documentation (this file)
└── LICENSE                # License file
```

## Prerequisites

### Tools
- **C++ Compiler**: GCC, Clang, or MSVC (e.g., via Visual Studio or MinGW).
- **CMake**: Version 3.10 or higher.
- **Git**: To clone the repository.

### Libraries
- **spdlog**: For logging.
- **Eigen**: For matrix and vector computations.

## Installation

### Clone the Repository
Clone the project repository:
```bash
git clone https://github.com/TUkan74/Software_renderer.git
cd Software-renderer
```

### Build the Project

#### **Windows (Using MinGW)**
1. Create a build directory:
   ```bash
   mkdir build && cd build
   ```

2. Configure the project with CMake:
   ```bash
   cmake .. -G "MinGW Makefiles"
   ```

3. Build the project:
   ```bash
   mingw32-make
   ```

4. Run the program:
   ```bash
   ./software-renderer.exe
   ```

#### **Linux**
1. Create a build directory:
   ```bash
   mkdir build && cd build
   ```

2. Configure the project with CMake:
   ```bash
   cmake .. -G "Unix Makefiles"
   ```

3. Build the project:
   ```bash
   make
   ```

4. Run the program:
   ```bash
   ./software-renderer
   ```

### Logging and Output
- The program generates a log file in the `logs/` directory (e.g., `logs/app.log`).
- Check the console for rendered output details.

## Adding Required Libraries

### spdlog

#### Option 1: Use FetchContent (Recommended)
The project is already configured to download `spdlog` automatically using CMake's `FetchContent` module. No manual setup is needed.

#### Option 2: Install Locally
If you want to install `spdlog` manually:
1. Clone and build `spdlog`:
   ```bash
   git clone https://github.com/gabime/spdlog.git
   cd spdlog
   mkdir build && cd build
   cmake ..
   make
   sudo make install
   ```
2. Ensure `spdlog` is available in your CMake configuration by setting the path to `spdlogConfig.cmake` if necessary:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/spdlog/build
   ```

### Eigen

#### Option 1: Use FetchContent (Recommended)
The project is configured to download Eigen using `FetchContent`. No manual setup is required.

#### Option 2: Install Locally
1. Clone Eigen:
   ```bash
   git clone https://gitlab.com/libeigen/eigen.git
   ```
2. Add the Eigen include path to your CMake configuration:
   ```bash
   cmake .. -DEigen3_DIR=/path/to/eigen
   ```

## Troubleshooting
- **spdlog Not Found**: Ensure that either `FetchContent` is enabled or the library is installed locally and its path is correctly set in `CMakeLists.txt`.
- **Eigen Not Found**: Verify that Eigen is either downloaded by `FetchContent` or its path is included during the CMake configuration.
- **CMake Errors**: Ensure you are using a compatible version of CMake (>=3.10).

## Future Enhancements
- Add support for z-buffering to handle depth in rendering.
- Implement phong shading for realistic lighting.
- Extend the renderer to support additional file formats like PLY or STL.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

