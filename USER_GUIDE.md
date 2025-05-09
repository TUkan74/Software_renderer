# Software Renderer - User Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Getting Started](#getting-started)
4. [Features](#features)
5. [Usage Examples](#usage-examples)
6. [Troubleshooting](#troubleshooting)

## Introduction

Software Renderer is a 3D rendering application that allows you to render 3D models with various rendering modes. It supports wireframe, solid, textured, and shaded rendering, making it suitable for both educational and practical 3D visualization purposes.

## Installation

### System Requirements
- **Hardware Requirements**:
  - CPU: Intel Core i3 or AMD Ryzen 3 or equivalent (minimum)
  - RAM: 4GB (minimum), 8GB (recommended)
  - GPU: Any GPU with DirectX 11 support (integrated graphics sufficient)
  - Storage: 100MB free space
- **Windows Requirements**:
  - Windows 10 or newer
  - Visual Studio 2022 with C++ Build Tools
  - CMake 3.10 or higher
- **Linux Requirements**:
  - Ubuntu 20.04+, Fedora 33+, or equivalent
  - GCC 9.0+ or Clang 10.0+
  - CMake 3.10 or higher
  - Make or Ninja build system

### Installation Steps

#### Windows

1. **Download the Software**
   - Clone the repository or download the source code

2. **Build the Application**
   ```batch
   scripts\build_windows.bat build
   ```
   This will create the executable in `build\Release\software-renderer.exe`

3. **Verify Installation**
   - Navigate to `build\Release`
   - Run `software-renderer.exe` to verify the installation

#### Linux

1. **Download the Software**
   - Clone the repository or download the source code

2. **Build the Application with Make**
   ```bash
   # Create build directory
   mkdir -p build && cd build
   
   # Configure with CMake
   cmake ..
   
   # Build the project
   make -j$(nproc)
   ```
   This will create the executable `software-renderer` in the `build` directory.

3. **Alternative: Build with Ninja (faster)**
   ```bash
   # Install Ninja if needed
   # sudo apt-get install ninja-build   # Debian/Ubuntu
   # sudo dnf install ninja-build       # Fedora
   
   # Create build directory
   mkdir -p build && cd build
   
   # Configure with Ninja
   cmake -G Ninja ..
   
   # Build the project
   ninja
   ```

4. **Verify Installation**
   - In the `build` directory
   - Run `./software-renderer` to verify the installation

## Getting Started

### Basic Usage

1. **Launch the Application**
   ```batch
   # Windows
   cd build\Release
   software-renderer.exe
   
   # Linux
   cd build
   ./software-renderer
   ```

2. **Load a 3D Model**
   - The application supports standard 3D model formats
   - Use the command line to specify input files:
   ```batch
   # Windows
   software-renderer.exe --input model.obj
   
   # Linux
   ./software-renderer --input model.obj
   ```

3. **Choose Rendering Mode**
   The renderer supports several modes:
   - Wireframe: Shows model edges
   - Solid: Filled triangles with basic shading
   - Textured: Applies textures to the model
   - Textured Shaded: Combines textures with lighting
   - Colorful: Random colors for visualization

### Application Interface

When the application is successfully launched and a model is loaded, you will see a render of your 3D model in the specified mode. The application outputs a rendered image to the specified file.

*Figure 1: Wireframe Mode - The model is displayed as a wireframe showing only edges and vertices*
![Wireframe Mode](images/wireframe_mode.png)

*Figure 2: Solid Mode - The model with basic diffuse lighting applied*
![Solid Mode](images/solid_mode.png)

*Figure 3: Textured Mode - The model with textures applied for a more realistic appearance*
![Textured Mode](images/textured_mode.png)

*Figure 4: Shaded Mode - Lighting is applied to the textured model creating depth and shadows*
![Shaded Mode](images/shaded_mode.png)

*Figure 5: Colorful Mode - Each face is assigned a random color, useful for visualization and debugging*
![Colorful Mode](images/colorful_mode.png)


**Note:** The application is command-line based and doesn't feature a graphical user interface. The rendered output is saved as an tga file that you can view using an tga image viewer.

## Features

### Rendering Modes

1. **Wireframe Mode**
   - Displays the model's wireframe structure
   - Useful for debugging and understanding model topology
   - Fast rendering for quick previews

2. **Solid Mode**
   - Fills triangles with solid colors
   - Includes basic diffuse lighting
   - Good for quick visualization

3. **Textured Mode**
   - Applies textures to the model
   - Supports UV mapping
   - Realistic surface appearance

4. **Textured Shaded Mode**
   - Combines textures with lighting
   - Supports normal mapping
   - Most realistic appearance

5. **Colorful Mode**
   - Assigns random colors to faces
   - Useful for debugging and visualization
   - Helps identify individual faces
   - Uses z-buffer for proper depth handling between triangles
   - Efficiently visualizes complex 3D models with correct occlusion

### Camera Controls

The renderer provides camera controls to view your model from different angles:
- Position: Adjust camera position
- Target: Set the point the camera looks at
- Up Vector: Define the camera's orientation

## Usage Examples

### Basic Rendering
```batch
# Windows
software-renderer.exe --input model.obj --mode wireframe

# Linux
./software-renderer --input model.obj --mode wireframe
```

### Textured Rendering
```batch
# Windows
software-renderer.exe --input model.obj --mode textured --texture texture.tga

# Linux
./software-renderer --input model.obj --mode textured --texture texture.tga
```

### Colorful Rendering
```batch
# Windows
software-renderer.exe --input model.obj --mode colorful

# Linux
./software-renderer --input model.obj --mode colorful
```

### Saving Output
```batch
# Windows
software-renderer.exe --input model.obj --output render.tga

# Linux
./software-renderer --input model.obj --output render.tga
```

## Troubleshooting

### Common Issues

1. **Application Won't Start**
   - **Windows**: 
     - Verify Visual Studio 2022 and C++ Build Tools are installed
     - Check if the executable exists in build\Release
     - Run as administrator if permission issues occur
   - **Linux**:
     - Verify required dependencies are installed (GCC/Clang, CMake)
     - Check if the executable has proper permissions (`chmod +x software-renderer`)
     - Verify shared libraries are available (`ldd software-renderer`)

2. **Model Not Loading**
   - Verify the model file exists
   - Check if the model format is supported
   - Ensure the model file is not corrupted
   - **Linux**: Remember to use forward slashes in file paths (`--input examples/model.obj`)

3. **Rendering Issues**
   - Check if textures are in the correct format (TGA)
   - Verify model has proper UV coordinates for textured modes
   - Ensure model has proper normals for shaded modes

### Getting Help

If you encounter issues not covered in this guide:
1. Check the error messages in the console output
2. Review the logs in the `logs` directory
3. Consult the developer documentation for technical details 