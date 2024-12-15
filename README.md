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
├── CMakeLists.txt         # CMake configuration file
└── README.md              # Project documentation (this file)

```

## Prerequisites

### Tools
- **Visual Studio 2022**: Ensure the C++ Build Tools workload is installed.
- **CMake**: Version 3.10 or higher.
- **Git**: To clone the repository.


## Installation

### Clone the Repository
Clone the project repository:
```bash
git clone https://github.com/TUkan74/Software_renderer.git
cd Software-renderer
```

## Build Instructions for SoftwareRenderer

### Winodws

This project includes a batch script `build_windows.bat` located in the `scripts` directory to streamline the build process on Windows.

#### Usage

##### Navigate to the Project Root Directory
Ensure you are in the root directory of the project (where `CMakeLists.txt` is located).

##### Available Commands

###### Build the Project
```cmd
scripts\build_windows.bat build
```
- Configures the project with CMake.
- Builds the project using the Visual Studio 2022 generator.
- Outputs the executable in `build\Release\software-renderer.exe`.

###### Clean the Build
```cmd
scripts\build_windows.bat clean
```
- Removes the build artifacts from the `build\Release` directory.

###### Clean All
```cmd
scripts\build_windows.bat clean all
```
- Deletes the entire `build` directory for a fresh start.

###### Rebuild the Project
```cmd
scripts\build_windows.bat rebuild
```
- Cleans the `build\Release` directory and rebuilds the project.

###### Rebuild All
```cmd
scripts\build_windows.bat rebuild all
```
- Deletes the entire `build` directory, reconfigures the project, and builds it from scratch.

#### Notes
- Run the commands from a Command Prompt or PowerShell window.
- Ensure all prerequisites are installed before running the script.


##### Logging and Output
- The program generates a log file in the `logs/` directory (e.g., `logs/app.log`).
- Check the console for rendered output details.

