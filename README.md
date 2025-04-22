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
│   ├── Model.h            # 3D model class
│   ├── Renderer.h         # Renderer class
│   ├── TestTexture.h      # Test texture generation
│   └── Texture.h          # Texture handling
├── src/                   # Source files
│   ├── Application.cpp    # Application implementation
│   ├── main.cpp           # Entry point of the application
│   ├── Model.cpp          # Model implementation
│   ├── Renderer.cpp       # Renderer implementation
│   ├── TestTexture.cpp    # Test texture generation implementation
│   └── Texture.cpp        # Texture implementation
├── scripts/               # Build and utility scripts
│   ├── build_windows.bat  # Windows build script
│   ├── setup_and_build.bat # Setup and build script that uses portable CMake
│   ├── simple_build_msvc.bat # Simple build script using MSVC directly
│   └── run_examples.bat   # Script to run examples
├── examples/              # Example 3D models and textures
│   └── cube.obj           # Sample cube 3D model
├── logs/                  # Directory for log files
├── CMakeLists.txt         # CMake configuration file
└── README.md              # Project documentation (this file)
```

## Prerequisites

### Tools
- **Visual Studio 2022**: Ensure the C++ Build Tools workload is installed.
- **CMake**: Version 3.10 or higher (automatically downloaded by setup script if needed).
- **Git**: To clone the repository.

## Build Instructions

### Windows - Quick Build

The simplest way to build the project is:

```cmd
scripts\build_windows.bat build
```

This will:
1. Check for CMake installation and use it if found
2. Configure the project with CMake using Visual Studio 2022 generator
3. Build the project in Release mode
4. Output the executable in `build\Release\software-renderer.exe`

If you encounter any issues with CMake, you can try:

```cmd
scripts\simple_build_msvc.bat
```

This bypasses CMake entirely and uses MSBuild directly.

### Troubleshooting Build Issues

If you encounter build errors:

1. **CMake Not Found**: The script will check common installation paths. If needed, install CMake from [cmake.org](https://cmake.org/download/)

2. **Visual Studio Issues**: Ensure Visual Studio 2022 with C++ build tools is installed

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

### Supported File Formats

- **3D Models**: Wavefront OBJ (.obj) files
- **Textures**: Uncompressed TGA (.tga) files
  - Supports Type 2 (uncompressed RGB/RGBA)
  - Supports Type 3 (uncompressed grayscale)
  - Does NOT support Type 10 (RLE compressed)

### Basic Examples

#### Generate Test Textures
```cmd
.\build\Release\software-renderer.exe --generate-test-textures
```

#### Render a Cube in Wireframe Mode
```cmd
.\build\Release\software-renderer.exe --input examples\cube.obj --output cube_wireframe.tga --mode wireframe
```

#### Render a Cube in Solid Mode
```cmd
.\build\Release\software-renderer.exe --input examples\cube.obj --output cube_solid.tga --mode solid
```

#### Render a Cube with Texture
```cmd
.\build\Release\software-renderer.exe --input examples\cube.obj --texture examples\checker_32.tga --output cube_textured.tga --mode textured
```

### Using Your Own Models

To use your own 3D models:

1. Place your OBJ file in the `examples` directory or any subdirectory
2. Run the software renderer with the path to your model:
   ```cmd
   .\build\Release\software-renderer.exe --input examples\your_model.obj --output your_model.tga --mode solid
   ```

If you want to use textures, make sure they are uncompressed TGA files (Type 2). Some modeling software exports compressed TGA files which won't work with this renderer.

### Viewing Output Files

The renderer outputs TGA files, which can be viewed with:
- [IrfanView](https://www.irfanview.com/) (Windows)
- [GIMP](https://www.gimp.org/) (cross-platform)
- [Paint.NET](https://www.getpaint.net/) (Windows)

You can also convert TGA files to more common formats like PNG or JPG using these programs.

## Development

### Adding New Models

Add your OBJ files to the `examples` directory. Models with high polygon counts may render slowly.

### Extending the Renderer

The core rendering logic is in `src/Renderer.cpp`. Key areas to extend:
- `renderWireframe()`: Wireframe rendering logic
- `renderSolid()`: Solid color rendering with basic lighting
- `renderTextured()`: Texture mapping implementation

## Common Issues

- **Upside-down Models**: Some OBJ files use different coordinate systems. The renderer is configured to display models properly.
- **Texture Loading Errors**: Check if your TGA file is compressed (Type 10). Only uncompressed TGA files (Type 2) are supported.
- **Performance**: The renderer is educational and not optimized for speed. Complex models may render slowly.

## License

This project is distributed under the MIT License.
