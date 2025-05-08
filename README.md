# Software Renderer

This project implements a basic **software renderer** in C++ from scratch. It focuses on rendering 3D models in the OBJ format without relying on external graphics libraries like OpenGL or DirectX. The software renderer is educational, offering features such as wireframe rendering, colored polygons, and texture mapping with TGA files.

## Features
- **OBJ File Parsing**: Parse and render 3D models stored in Wavefront OBJ format.
- **Basic Rendering**: Render models using wireframe, solid color, or texture mapping techniques.
- **Matrix Operations**: Utilize Eigen library for matrix and vector computations.
- **Logging**: Use spdlog for detailed logging of the rendering process.
- **Test Assets**: Generate test textures and use sample 3D models.

## Project Structure
```
software-renderer/
├── build/                 # Directory for build artifacts
├── include/               # Header files
│   ├── Application.h      # Main application class
│   ├── CommandLineParser.h # Command line parsing
│   ├── Model.h            # 3D model class
│   ├── Renderer.h         # Renderer class
│   ├── TestTexture.h      # Test texture generation
│   └── Texture.h          # Texture handling
├── src/                   # Source files
│   ├── Application.cpp    # Application implementation
│   ├── CommandLineParser.cpp # Command line parsing implementation
│   ├── main.cpp           # Entry point of the application
│   ├── Model.cpp          # Model implementation
│   ├── Renderer.cpp       # Renderer implementation
│   ├── TestTexture.cpp    # Test texture generation implementation
│   └── Texture.cpp        # Texture implementation
├── scripts/               # Build and utility scripts
│   ├── build_windows.bat  # Windows build script
│   └── run_examples.bat   # Script to run examples
├── examples/              # Example 3D models and textures
│   └── cube.obj           # Sample cube 3D model
├── logs/                  # Directory for log files
├── CMakeLists.txt         # CMake configuration file
├── DEVELOPER_GUIDE.md     # Guide for developers
├── USER_GUIDE.md          # Guide for users
└── README.md              # Project documentation (this file)
```

## Prerequisites

### System Requirements
- **Hardware Requirements**:
  - CPU: Intel Core i3 or AMD Ryzen 3 or equivalent (minimum)
  - RAM: 4GB (minimum), 8GB (recommended)
  - GPU: Any GPU with DirectX 11 support (integrated graphics sufficient)
  - Storage: 100MB free space

### Windows
- **Windows 10 or newer**
- **Visual Studio 2022**: Ensure the C++ Build Tools workload is installed.
- **CMake**: Version 3.10 or higher (automatically downloaded by setup script if needed).
- **Git**: To clone the repository.

### Linux
- **Ubuntu 20.04 or newer / Fedora 33 or newer**
- **GCC 9.0+ or Clang 10.0+**
- **CMake**: Version 3.10 or higher
- **Make or Ninja**: Build system
- **Git**: To clone the repository

## Build Instructions

### Windows
The simplest way to build the project on Windows is:

```cmd
scripts\build_windows.bat build
```

This will:
1. Check for CMake installation and use it if found
2. Configure the project with CMake using Visual Studio 2022 generator
3. Build the project in Release mode
4. Output the executable in `build\Release\software-renderer.exe`

#### Additional Build Options
- Clean build: `scripts\build_windows.bat clean`
- Rebuild: `scripts\build_windows.bat rebuild`
- Full rebuild: `scripts\build_windows.bat rebuild all`

### Linux
To build the project on Linux, use standard CMake commands:

#### Using Make
```bash
# Create build directory
mkdir -p build && cd build

# Configure
cmake ..

# Build
make -j$(nproc)

# Run
./software-renderer
```

#### Using Ninja (faster builds)
```bash
# Install ninja if not present
# sudo apt-get install ninja-build (Ubuntu/Debian)
# sudo dnf install ninja-build (Fedora)

# Create build directory
mkdir -p build && cd build

# Configure with Ninja
cmake -G Ninja ..

# Build
ninja

# Run
./software-renderer
```

### Troubleshooting Build Issues

If you encounter build errors:

1. **CMake Not Found**: 
   - Windows: The script will check common installation paths or install it automatically
   - Linux: Install via package manager (`apt-get install cmake` or `dnf install cmake`)

2. **Compiler Issues**: 
   - Windows: Ensure Visual Studio 2022 with C++ build tools is installed
   - Linux: Install GCC/Clang via package manager

3. **Build Failures**: Check the `logs/app.log` file for detailed error messages

## Usage

The software renderer supports the following command-line arguments:

```
Software Renderer - A simple 3D rendering engine
Usage: software-renderer [options]
Options:
  --help                   Display this help message
  --input <obj_file>       Input OBJ model file (required unless --generate-test-textures is used)
  --texture <tga_file>     Input TGA texture file
  --output <tga_file>      Output TGA image file (default: output.tga)
  --width <pixels>         Width of the output image (default: 800)
  --height <pixels>        Height of the output image (default: 600)
  --mode <mode>            Rendering mode (default: wireframe)
                           Modes: wireframe, solid, textured, shaded
  --camera-x <value>       Camera X position (default: 0)
  --camera-y <value>       Camera Y position (default: 0)
  --camera-z <value>       Camera Z position (default: 5)
  --generate-test-textures Generate test textures in the examples directory
```

### Basic Examples

#### Generate Test Textures
```cmd
# Windows
.\build\Release\software-renderer.exe --generate-test-textures

# Linux
./build/software-renderer --generate-test-textures
```

#### Render a Cube in Wireframe Mode
```cmd
# Windows
.\build\Release\software-renderer.exe --input examples\cube.obj --output cube_wireframe.tga --mode wireframe

# Linux
./build/software-renderer --input examples/cube.obj --output cube_wireframe.tga --mode wireframe
```

### Using Your Own Models

To use your own 3D models:

1. Place your OBJ file in the `examples` directory or any subdirectory
2. Run the software renderer with the path to your model:
   ```cmd
   # Windows
   .\build\Release\software-renderer.exe --input examples\your_model.obj --output your_model.tga --mode solid
   
   # Linux
   ./build/software-renderer --input examples/your_model.obj --output your_model.tga --mode solid
   ```

## Documentation

For more detailed information:
- **User Guide**: See [USER_GUIDE.md](USER_GUIDE.md) for comprehensive usage instructions
- **Developer Guide**: See [DEVELOPER_GUIDE.md](DEVELOPER_GUIDE.md) for architecture details, class diagrams, and extending the renderer

## License

This project is distributed under the MIT License.
