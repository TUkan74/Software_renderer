@echo off
setlocal EnableDelayedExpansion

echo Software Renderer - Run Examples
echo ==============================
echo.

REM Check if the build directory exists
if not exist build\Release\software-renderer.exe (
    echo Error: software-renderer.exe not found.
    echo Run scripts\setup_and_build.bat first to build the project.
    exit /b 1
)

REM Create examples directory if it doesn't exist
if not exist examples (
    mkdir examples
    echo Created examples directory.
)

REM Generate test textures
echo Generating test textures...
build\Release\software-renderer.exe --generate-test-textures
if %ERRORLEVEL% NEQ 0 (
    echo Failed to generate test textures.
    exit /b 1
)

REM Check if cube.obj exists
if not exist examples\cube.obj (
    echo Warning: examples\cube.obj not found.
    echo You may need to create this file manually.
) else (
    REM Render cube in different modes
    echo.
    echo Rendering cube in wireframe mode...
    build\Release\software-renderer.exe --input examples\cube.obj --output cube_wireframe.tga --mode wireframe
    
    echo Rendering cube in solid mode...
    build\Release\software-renderer.exe --input examples\cube.obj --output cube_solid.tga --mode solid
    
    echo Rendering cube with texture (checker pattern)...
    build\Release\software-renderer.exe --input examples\cube.obj --texture examples\checker_32.tga --output cube_textured.tga --mode textured
    
    echo Rendering cube with texture and shading...
    build\Release\software-renderer.exe --input examples\cube.obj --texture examples\gradient.tga --output cube_textured_shaded.tga --mode shaded
    
    echo.
    echo All examples rendered successfully.
    echo Output files:
    echo   - cube_wireframe.tga
    echo   - cube_solid.tga
    echo   - cube_textured.tga
    echo   - cube_textured_shaded.tga
)

echo.
echo Examples completed successfully.
exit /b 0 