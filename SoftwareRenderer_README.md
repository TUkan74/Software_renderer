
# SoftwareRenderer

SoftwareRenderer is a C++ project that implements a software-based renderer. It leverages modern C++ standards and utilizes external libraries such as spdlog for logging and Eigen for linear algebra operations.

## Table of Contents
- Prerequisites
- Project Structure
- Building the Project
  - Build Commands
- Cleaning the Build
  - Clean Commands
- Rebuilding the Project
  - Rebuild Commands
- Running the Program
- Troubleshooting
- License

## Prerequisites
Before building and running SoftwareRenderer, ensure that your development environment meets the following requirements:

### Operating System:
- Windows 10 or later (64-bit)

### Development Tools:
- Visual Studio 2022 (Community, Professional, or Enterprise Edition)
  - Ensure that the C++ Build Tools workload is installed.
- CMake (version 3.10 or higher)

### Internet Connection:
- Required for CMake's FetchContent to download external dependencies (spdlog and Eigen).

### Environment Variables:
- MSBuild should be accessible via the PATH environment variable. The build script assists in locating msbuild.exe.

## Project Structure
Here's an overview of the project's directory structure:

```
Software_renderer/
├── CMakeLists.txt
├── include/
│   └── ...             # Header files
├── src/
│   └── ...             # Source files (.cpp)
├── scripts/
│   └── build_windows.bat  # Build script for Windows
├── build/              # Build directory (generated after building)
├── README.md
└── ...                 # Other project files
```

- **CMakeLists.txt**: Defines the build configuration for the project.
- **include/**: Contains header files for the project.
- **src/**: Contains source (.cpp) files.
- **scripts/**: Contains the build script `build_windows.bat` for Windows.
- **build/**: Generated directory where build artifacts are stored.

## Building the Project
The project uses a batch script `build_windows.bat` to streamline the build process on Windows. This script handles configuration, building, and managing dependencies using CMake.

### Build Commands
Navigate to the root directory of the project (`Software_renderer`) using Command Prompt or PowerShell, and execute the following commands based on your needs:

#### Build the Project:
```batch
scripts\build_windows.bat build
```

#### What It Does:
- **Checks for MSVC Compiler (cl.exe)**: Ensures that the MSVC compiler is available.
- **Checks for CMake**: Installs CMake if it's not already installed.
- **Creates `build` Directory**: If it doesn't exist.
- **Configures the Project with CMake**: Uses the Visual Studio 17 2022 generator.
- **Builds the Project**: Compiles the project in the Release configuration.

#### Output:
Upon successful build, the `build` directory will contain the generated Visual Studio solution (`SoftwareRenderer.sln`) and executable (`software-renderer.exe`).

## Cleaning the Build
Cleaning the build removes the generated executables and intermediate build files. There are two cleaning options: Standard Clean and Clean All.

### Clean Commands
#### Standard Clean:
```batch
scripts\build_windows.bat clean
```

#### What It Does:
- **Navigates to the `build` Directory**: Ensures operations are performed in the correct context.
- **Lists Contents of `build`**: For verification.
- **Cleans the Entire Build Solution**: Uses msbuild.exe to clean both Release and Debug configurations.

#### Note:
This command cleans the build artifacts without removing the `build` directory itself.

#### Clean All:
```batch
scripts\build_windows.bat clean all
```

#### What It Does:
- **Removes the Entire `build` Directory**: Deletes all build artifacts and configurations.
- **Provides a Fresh Start**: Ideal for resetting the build state.

#### Note:
After performing a Clean All, you'll need to rebuild the project from scratch.

## Rebuilding the Project
Rebuilding involves cleaning the current build artifacts and then building the project anew. Similar to cleaning, there are two rebuild options: Standard Rebuild and Rebuild All.

### Rebuild Commands
#### Standard Rebuild:
```batch
scripts\build_windows.bat rebuild
```

#### What It Does:
- **Performs a Standard Clean**: Cleans the build solution (Release and Debug configurations).
- **Builds the Project**: Compiles the project in the Release configuration.

#### Rebuild All:
```batch
scripts\build_windows.bat rebuild all
```

#### What It Does:
- **Performs a Clean All**: Removes the entire build directory.
- **Builds the Project**: Configures and compiles the project from scratch.

#### Note:
Useful when you want to ensure that all build artifacts are freshly generated.

## Running the Program
After successfully building the project, you can run the SoftwareRenderer executable.

#### Navigate to the Executable:
```batch
cd build\Release
```

#### Run the Executable:
```batch
software-renderer.exe
```

#### Usage:
The program may accept command-line arguments depending on its implementation. Refer to the program's documentation or source code for specific usage instructions.

#### Example:
```batch
software-renderer.exe --input scene.obj --output render.png
```

#### Note:
Replace the arguments with actual parameters as required by your application.

## Troubleshooting
If you encounter issues during the build, clean, or run processes, consider the following steps:

### Ensure Prerequisites are Met:
- **Visual Studio 2022**: Confirm that it's installed with the C++ Build Tools workload.
- **CMake**: Verify that CMake is installed. The build script attempts to install it if missing, but manual installation can help resolve path issues.

### Verify Environment Variables:
- **MSBuild Path**: Ensure that msbuild.exe is accessible via the PATH. You can verify by running:
  ```batch
  where msbuild
  ```

#### Expected Output:
```batch
C:\Program Files\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe
```

If not found, refer to the Build Steps to add MSBuild to your PATH.

### Run from Developer Command Prompt:
- **Advantages**: Automatically sets up environment variables for MSVC and MSBuild.
- **How to Open**:
  Press `Win`, type "Developer Command Prompt for VS 2022", and open it.

### Check CMake Configuration:
- Ensure that `CMakeLists.txt` is correctly configured.
- Verify that all dependencies are specified and accessible.

### Permissions Issues:
- Run Command Prompt or PowerShell as Administrator to avoid permission-related errors during build or clean operations.

### Review Build Script Output:
- The `build_windows.bat` script provides diagnostic messages. Review them to identify where the process is failing.

### Delete `build` Directory Manually:
- If the build or clean process is stuck, manually delete the `build` directory and rebuild.

### Consult Logs:
- Check any generated logs for detailed error messages.

## License
MIT License

## Additional Notes
- **Extensibility**: The `build_windows.bat` script can be extended to support additional build configurations or targets as your project evolves.
- **Cross-Platform Support**: While the current build script is tailored for Windows, consider implementing build scripts for other operating systems (e.g., Bash scripts for Linux/macOS) to enhance portability.
- **Continuous Integration**: Integrate the build process with CI/CD pipelines (e.g., GitHub Actions, Jenkins) for automated building and testing.

Feel free to customize this README.md further to include more specific details about your project, such as features, screenshots, or contribution guidelines.
