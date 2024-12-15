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
        set "E+*--XEC_NAME=software-renderer.exe"

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

        popd
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
            set "E+*--XEC_NAME=software-renderer.exe"

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

            popd
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


REM Verify CMake installation
cmake --version >nul 2>&1
if !ERRORLEVEL! NEQ 0 (
    echo CMake installation failed.
    goto :end
)

REM Create and enter build directory
if not exist build mkdir build
cd build

REM Configure the project with CMake
echo Configuring the project with CMake...
cmake -G "Visual Studio 17 2022" ..
if !ERRORLEVEL! NEQ 0 (
    echo CMake configuration failed.
    goto :end
)

REM Build the project with CMake
echo Building the project with CMake...
cmake --build . --config Release
if !ERRORLEVEL! NEQ 0 (
    echo Build failed.
    goto :end
)

echo Build completed successfully.
goto :end