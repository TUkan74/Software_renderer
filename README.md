# 3D Software Renderer

## Overview
The **3D Software Renderer** is a basic rendering program implemented in C++. It demonstrates a minimalistic software rendering pipeline, using OBJ files for 3D model input and designed for educational and demonstration purposes. The project is built with **CMake** and supports compilation on both Linux and Windows platforms.

---

## Project Structure
```
3D_SOFTWARE_RENDERER/
├── build/                 # Build artifacts (created automatically by CMake)
├── include/               # Header files
├── src/                   # Source files
│   ├── main.cpp           # Entry point of the program
├── logs/                  # Optional logs directory
├── models/                # Optional directory for OBJ models
├── tests/                 # Optional directory for unit tests
├── CMakeLists.txt         # CMake configuration file
├── .gitignore             # Git ignore file
└── README.md              # This documentation file
```

---

## Prerequisites

### Tools Required:
- **CMake** (minimum version 3.10)
- **C++17-compatible compiler**
  - Linux: `g++`
  - Windows: `MinGW` or `Visual Studio 2019+`

### Install CMake:
- **Linux:**
  ```bash
  sudo apt install cmake
  ```
- **Windows:**
  Download and install CMake from [cmake.org](https://cmake.org/download/).

---

## Build Instructions

### Linux
1. Open a terminal and navigate to the project root:
   ```bash
   cd 3D_SOFTWARE_RENDERER
   ```
2. Create a build directory and navigate into it:
   ```bash
   mkdir -p build
   cd build
   ```
3. Run CMake to generate the build files:
   ```bash
   cmake ..
   ```
4. Compile the project:
   ```bash
   make
   ```

### Windows
1. Open a terminal or Command Prompt in the project root:
   ```cmd
   cd 3D_SOFTWARE_RENDERER
   ```
2. Create a build directory and navigate into it:
   ```cmd
   mkdir build
   cd build
   ```
3. Run CMake to generate build files:
   - For **MinGW**:
     ```cmd
     cmake .. -G "MinGW Makefiles"
     ```
   - For **Visual Studio**:
     ```cmd
     cmake .. -G "Visual Studio 16 2019"
     ```
4. Build the project:
   - For **MinGW**:
     ```cmd
     mingw32-make
     ```
   - For **Visual Studio**:
     Open the generated `.sln` file in Visual Studio and build the solution.

---

## Running the Program

### Linux:
```bash
./build/renderer
```

### Windows:
- **MinGW:**
  ```cmd
  .\build\renderer.exe
  ```
- **Visual Studio:**
  Run the executable from the `build/Debug` or `build/Release` folder, depending on the build configuration.

### Expected Output (For DEMO) :
```plaintext
3D Software Renderer - Demo Build
Model is initialized and working!
```

---

## Cleaning the Project

### Using CMake Clean Target:
1. Navigate to the `build` directory:
   ```bash
   cd build
   ```
2. Run the clean-all target:
   ```bash
   cmake --build . --target clean-all
   ```

### Manual Cleanup:
- **Linux:**
  ```bash
  rm -rf build
  ```
- **Windows (PowerShell):**
  ```powershell
  Remove-Item -Recurse -Force build
  ```

---

## Extending the Project
This project is designed to be modular and extensible. You can:
- Add new source files to `src/` and corresponding header files to `include/`.
- Update the `CMakeLists.txt` file dynamically detects all `.cpp` files in `src/`.
- Use the `models/` directory to test the program with custom OBJ files.
- Implement additional functionality like texture mapping, lighting, or more complex rendering algorithms.

---

## .gitignore
```
# Ignore build artifacts
/build/

# CMake-generated files
CMakeFiles/
CMakeCache.txt
cmake_install.cmake
Makefile

# Logs
logs/*

# Miscellaneous
*.tmp
*.swp
*.log
*.exe
*.out
```

---

## License
This project is open-source and available under the MIT License. See the LICENSE file for more details.

---

## Acknowledgments
This project structure and documentation are tailored for educational purposes and to demonstrate C++ software engineering practices.

