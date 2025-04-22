
# Build Instructions for SoftwareRenderer

This project includes a batch script `build_windows.bat` located in the `scripts` directory to streamline the build process on Windows.

## Usage

### Navigate to the Project Root Directory
Ensure you are in the root directory of the project (where `CMakeLists.txt` is located).

### Available Commands

#### Build the Project
```cmd
scripts\build_windows.bat build
```
- Configures the project with CMake.
- Builds the project using the Visual Studio 2022 generator.
- Outputs the executable in `build\Release\software-renderer.exe`.

#### Clean the Build
```cmd
scripts\build_windows.bat clean
```
- Removes the build artifacts from the `build\Release` directory.

#### Clean All
```cmd
scripts\build_windows.bat clean all
```
- Deletes the entire `build` directory for a fresh start.

#### Rebuild the Project
```cmd
scripts\build_windows.bat rebuild
```
- Cleans the `build\Release` directory and rebuilds the project.

#### Rebuild All
```cmd
scripts\build_windows.bat rebuild all
```
- Deletes the entire `build` directory, reconfigures the project, and builds it from scratch.

## Requirements
- **Visual Studio 2022**: Ensure the C++ Build Tools workload is installed.
- **CMake**: Version 3.10 or higher.

## Notes
- Run the commands from a Command Prompt or PowerShell window.
- Ensure all prerequisites are installed before running the script.
