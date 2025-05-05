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
- Visual Studio 2022 with C++ Build Tools
- CMake 3.10 or higher

### Installation Steps

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

## Getting Started

### Basic Usage

1. **Launch the Application**
   ```batch
   cd build\Release
   software-renderer.exe
   ```

2. **Load a 3D Model**
   - The application supports standard 3D model formats
   - Use the command line to specify input files:
   ```batch
   software-renderer.exe --input model.obj
   ```

3. **Choose Rendering Mode**
   The renderer supports several modes:
   - Wireframe: Shows model edges
   - Solid: Filled triangles with basic shading
   - Textured: Applies textures to the model
   - Textured Shaded: Combines textures with lighting
   - Colorful: Random colors for visualization

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
software-renderer.exe --input model.obj --mode wireframe
```

### Textured Rendering
```batch
software-renderer.exe --input model.obj --mode textured --texture texture.tga
```

### Colorful Rendering
```batch
software-renderer.exe --input model.obj --mode colorful
```

### Saving Output
```batch
software-renderer.exe --input model.obj --output render.tga
```

## Troubleshooting

### Common Issues

1. **Application Won't Start**
   - Verify Visual Studio 2022 and C++ Build Tools are installed
   - Check if the executable exists in build\Release
   - Run as administrator if permission issues occur

2. **Model Not Loading**
   - Verify the model file exists
   - Check if the model format is supported
   - Ensure the model file is not corrupted

3. **Rendering Issues**
   - Check if textures are in the correct format (TGA)
   - Verify model has proper UV coordinates for textured modes
   - Ensure model has proper normals for shaded modes

### Getting Help

If you encounter issues not covered in this guide:
1. Check the error messages in the console output
2. Review the logs in the `logs` directory
3. Consult the developer documentation for technical details 