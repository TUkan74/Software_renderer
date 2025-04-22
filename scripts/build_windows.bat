@echo off
setlocal EnableDelayedExpansion

REM ===================================================================
REM Display the received arguments for debugging
echo Received arguments: [%~1] [%~2]
echo.

REM ===================================================================
REM Function to display usage
if "%~1"=="" goto :usage

REM ===================================================================
REM Handle 'clean', 'clean all', 'rebuild', and 'rebuild all'
if /I "%~1"=="clean" (
    if /I "%~2"=="all" (
        echo Performing 'clean all'...
        if exist build (
            echo Removing entire build directory: build
            rmdir /s /q build
            if !ERRORLEVEL! EQU 0 (
                echo Build directory removed successfully.
            ) else (
                echo Failed to remove build directory.
            )
        ) else (
            echo Build directory does not exist.
        )
        goto :end
    ) else (
        set "BUILD_DIR=build\Release"
        set "EXEC_NAME=software-renderer.exe"

        echo Checking for executable in: !BUILD_DIR!
        if exist "!BUILD_DIR!\!EXEC_NAME!" (
            echo Found !BUILD_DIR!\!EXEC_NAME! - attempting to delete.
            del /f /q "!BUILD_DIR!\!EXEC_NAME!"
            if !ERRORLEVEL! EQU 0 (
                echo !EXEC_NAME! deleted successfully.
            ) else (
                echo Failed to delete !EXEC_NAME!.
            )
        ) else (
            echo !EXEC_NAME! not found in !BUILD_DIR!.
        )

        echo.
        echo Clean operation completed.
        goto :end
    )
)

if /I "%~1"=="rebuild" (
    if /I "%~2"=="all" (
        echo Performing 'rebuild all'...
        REM -----------------------------------------------------------
        REM Perform 'clean all'
        if exist build (
            echo Removing entire build directory: build
            rmdir /s /q build
            if !ERRORLEVEL! EQU 0 (
                echo Build directory removed successfully.
            ) else (
                echo Failed to remove build directory.
            )
        ) else (
            echo Build directory does not exist.
        )
        
        REM -----------------------------------------------------------
        REM Now perform the 'build' operation
        echo Starting build after clean all...
        call :build_project
        goto :end
    ) else (
        echo Performing 'rebuild'...
        
        REM -----------------------------------------------------------
        REM Perform 'clean'
        if not exist build (
            echo Build directory does not exist. Nothing to clean.
        ) else (
            set "BUILD_DIR=build\Release"
            set "EXEC_NAME=software-renderer.exe"

            echo Checking for executable in: !BUILD_DIR!
            if exist "!BUILD_DIR!\!EXEC_NAME!" (
                echo Found !BUILD_DIR!\!EXEC_NAME! - attempting to delete.
                del /f /q "!BUILD_DIR!\!EXEC_NAME!"
                if !ERRORLEVEL! EQU 0 (
                    echo !EXEC_NAME! deleted successfully.
                ) else (
                    echo Failed to delete !EXEC_NAME!.
                )
            ) else (
                echo !EXEC_NAME! not found in !BUILD_DIR!.
            )
        )
        
        REM -----------------------------------------------------------
        REM Now build the project
        echo Starting build after clean...
        call :build_project
        goto :end
    )
)

REM ===================================================================
REM Handle 'build' argument
if /I "%~1"=="build" (
    echo Building the project...
    call :build_project
    goto :end
)

REM ===================================================================
REM If none of the above matched, show usage
:usage
echo Usage: build_windows.bat [build ^| clean ^| clean all ^| rebuild ^| rebuild all]
echo(
echo build         - Configure and build the project.
echo clean         - Clean the entire build solution.
echo clean all     - Remove the entire build directory.
echo rebuild       - Clean the entire build solution and then build.
echo rebuild all   - Remove the entire build directory and then build.
goto :end

REM ===================================================================
REM Function to build the project
:build_project

REM Find and verify CMake installation
echo Checking for CMake...

REM Try direct cmake command first
cmake --version >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    echo CMake not found in PATH. Checking common installation locations...
    
    REM Define CMake paths individually to avoid issues with spaces
    set "CMAKE_PATH1=C:\Program Files\CMake\bin\cmake.exe"
    set "CMAKE_PATH2=C:\Program Files (x86)\CMake\bin\cmake.exe"
    set "CMAKE_PATH3=%LOCALAPPDATA%\Programs\CMake\bin\cmake.exe"
    
    if exist "!CMAKE_PATH1!" (
        echo Found CMake at: !CMAKE_PATH1!
        set "CMAKE_EXE=!CMAKE_PATH1!"
        goto :cmake_found
    )
    
    if exist "!CMAKE_PATH2!" (
        echo Found CMake at: !CMAKE_PATH2!
        set "CMAKE_EXE=!CMAKE_PATH2!"
        goto :cmake_found
    )
    
    if exist "!CMAKE_PATH3!" (
        echo Found CMake at: !CMAKE_PATH3!
        set "CMAKE_EXE=!CMAKE_PATH3!"
        goto :cmake_found
    )
    
    echo.
    echo ERROR: CMake not found. Please install CMake and ensure it's in your PATH.
    echo You can download CMake from: https://cmake.org/download/
    echo.
    echo After installing, you may need to:
    echo 1. Restart your command prompt, or
    echo 2. Add CMake to your PATH manually, or
    echo 3. Use the full path to cmake.exe
    echo.
    goto :end
) else (
    echo CMake found in PATH.
    set "CMAKE_EXE=cmake"
)

:cmake_found

REM Create and enter build directory
if not exist build mkdir build
cd build

REM Configure the project with CMake
echo Configuring the project with CMake...

if "!CMAKE_EXE!"=="cmake" (
    cmake -G "Visual Studio 17 2022" ..
) else (
    "!CMAKE_EXE!" -G "Visual Studio 17 2022" ..
)

if !ERRORLEVEL! NEQ 0 (
    echo CMake configuration failed.
    cd ..
    goto :end
)

REM Build the project with CMake
echo Building the project with CMake...

if "!CMAKE_EXE!"=="cmake" (
    cmake --build . --config Release
) else (
    "!CMAKE_EXE!" --build . --config Release
)

if !ERRORLEVEL! NEQ 0 (
    echo Build failed.
    cd ..
    goto :end
)

echo Build completed successfully.
cd ..
goto :end

:end
exit /B 0