@echo off
setlocal EnableDelayedExpansion

echo Software Renderer - Setup and Build Script
echo =========================================
echo.

REM Check if CMake is installed
cmake --version >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed. Attempting to download portable CMake...
    echo.
    
    REM Create temp directory for downloads
    if not exist temp mkdir temp
    cd temp
    
    REM Download portable CMake zip
    echo Downloading portable CMake...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-windows-x86_64.zip' -OutFile 'cmake-portable.zip'}"
    
    if !ERRORLEVEL! NEQ 0 (
        echo Failed to download CMake. Trying alternative method...
        echo Using curl to download CMake...
        curl -L "https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-windows-x86_64.zip" -o cmake-portable.zip
        
        if !ERRORLEVEL! NEQ 0 (
            echo Failed to download CMake using curl too.
            goto :error
        )
    )
    
    echo Extracting portable CMake...
    powershell -Command "& {Expand-Archive -Path 'cmake-portable.zip' -DestinationPath '.' -Force}"
    
    if !ERRORLEVEL! NEQ 0 (
        echo Failed to extract CMake.
        goto :error
    )
    
    echo Setting up portable CMake...
    
    REM Get the full path to the cmake exe
    set "CMAKE_EXE=%CD%\cmake-3.26.4-windows-x86_64\bin\cmake.exe"
    
    if not exist "!CMAKE_EXE!" (
        echo Could not find cmake.exe in the extracted package.
        echo Expected location: !CMAKE_EXE!
        dir "%CD%\cmake-3.26.4-windows-x86_64\bin"
        goto :error
    )
    
    echo Found CMake executable at: !CMAKE_EXE!
    
    cd ..
) else (
    echo CMake is already installed.
    set "CMAKE_EXE=cmake"
)

echo.
echo Building Software Renderer...
echo.

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Configure the project with CMake
echo Configuring the project with CMake...
cd build

if "%CMAKE_EXE%"=="cmake" (
    cmake -G "Visual Studio 17 2022" ..
) else (
    "!CMAKE_EXE!" -G "Visual Studio 17 2022" ..
)

if %ERRORLEVEL% NEQ 0 (
    echo CMake configuration failed.
    goto :error
)

REM Build the project with CMake
echo Building the project with CMake...

if "%CMAKE_EXE%"=="cmake" (
    cmake --build . --config Release
) else (
    "!CMAKE_EXE!" --build . --config Release
)

if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    goto :error
)

echo Build completed successfully.
cd ..

echo.
echo =========================================
echo Setup and build completed successfully!
echo.
echo You can now run the software renderer with example data:
echo   build\Release\software-renderer.exe --generate-test-textures
echo   build\Release\software-renderer.exe --input examples\cube.obj --output output.tga
echo.

goto :end

:error
echo.
echo Setup or build failed. Please check the error messages above.
cd ..
exit /b 1

:end
exit /b 0 