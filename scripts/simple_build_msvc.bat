@echo off
setlocal EnableDelayedExpansion

echo Software Renderer - Simple MSVC Build Script
echo =========================================
echo.

REM Find Visual Studio installation
echo Looking for Visual Studio installation...

REM Try to find vswhere.exe which comes with Visual Studio
set "VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE_PATH!" (
    echo vswhere.exe not found. Trying fallback path...
    set "VSWHERE_PATH=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
)

if not exist "!VSWHERE_PATH!" (
    echo Could not find vswhere.exe. Trying to find MSBuild manually...
    
    REM Check common MSBuild locations
    set "MSBUILD_LOCATIONS=^
        C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe^
        C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    
    for %%M in (!MSBUILD_LOCATIONS!) do (
        if exist "%%M" (
            set "MSBUILD_PATH=%%M"
            echo Found MSBuild at: !MSBUILD_PATH!
            goto :found_msbuild
        )
    )
    
    echo ERROR: Could not find MSBuild. Make sure Visual Studio is installed.
    goto :error
) else (
    echo Found vswhere.exe at: !VSWHERE_PATH!
    
    REM Use vswhere to find the latest Visual Studio installation
    for /f "usebackq tokens=*" %%i in (`"!VSWHERE_PATH!" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
        set "MSBUILD_PATH=%%i"
        echo Found MSBuild at: !MSBUILD_PATH!
        goto :found_msbuild
    )
    
    echo ERROR: Could not find MSBuild using vswhere. Make sure Visual Studio is installed.
    goto :error
)

:found_msbuild

echo.
echo Creating simple Visual Studio project...

REM Create directories if they don't exist
if not exist build mkdir build
if not exist logs mkdir logs
if not exist examples mkdir examples

REM Create a simple Visual Studio project file
set "PROJECT_DIR=%CD%\build\SimpleVS"
if not exist "!PROJECT_DIR!" mkdir "!PROJECT_DIR!"

REM Get all source and header files
set "SOURCE_FILES="
for %%F in (src\*.cpp) do (
    set "SOURCE_FILES=!SOURCE_FILES! %%F"
)

echo Creating MSVC project in: !PROJECT_DIR!

REM Create a minimal .vcxproj file
echo ^<?xml version="1.0" encoding="utf-8"?^> > "!PROJECT_DIR!\software-renderer.vcxproj"
echo ^<Project DefaultTargets="Build" ToolsVersion="17.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<ItemGroup Label="ProjectConfigurations"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<ProjectConfiguration Include="Release|x64"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<Configuration^>Release^</Configuration^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<Platform^>x64^</Platform^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^</ProjectConfiguration^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^</ItemGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<PropertyGroup Label="Globals"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<VCProjectVersion^>17.0^</VCProjectVersion^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<ProjectGuid^>{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}^</ProjectGuid^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<Keyword^>Win32Proj^</Keyword^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<RootNamespace^>SoftwareRenderer^</RootNamespace^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^</PropertyGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" /^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<ConfigurationType^>Application^</ConfigurationType^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<UseDebugLibraries^>false^</UseDebugLibraries^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<PlatformToolset^>v143^</PlatformToolset^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<CharacterSet^>Unicode^</CharacterSet^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^</PropertyGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" /^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<LinkIncremental^>false^</LinkIncremental^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<OutDir^>$(SolutionDir)$(Platform)\$(Configuration)\^</OutDir^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<IntDir^>$(Platform)\$(Configuration)\^</IntDir^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^</PropertyGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'"^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<ClCompile^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<PreprocessorDefinitions^>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)^</PreprocessorDefinitions^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<AdditionalIncludeDirectories^>../../include;%(AdditionalIncludeDirectories)^</AdditionalIncludeDirectories^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<LanguageStandard^>stdcpp20^</LanguageStandard^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^</ClCompile^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^<Link^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<GenerateDebugInformation^>true^</GenerateDebugInformation^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<SubSystem^>Console^</SubSystem^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo       ^<OptimizeReferences^>true^</OptimizeReferences^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo     ^</Link^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^</ItemDefinitionGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<ItemGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"

REM Add all source files
for %%F in (src\*.cpp) do (
    echo     ^<ClCompile Include="..\..\%%F" /^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
)

echo   ^</ItemGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<ItemGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"

REM Add all header files
for %%F in (include\*.h) do (
    echo     ^<ClInclude Include="..\..\%%F" /^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
)

echo   ^</ItemGroup^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo   ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /^> >> "!PROJECT_DIR!\software-renderer.vcxproj"
echo ^</Project^> >> "!PROJECT_DIR!\software-renderer.vcxproj"

echo.
echo Building the project with MSBuild...
"!MSBUILD_PATH!" "!PROJECT_DIR!\software-renderer.vcxproj" /p:Configuration=Release /p:Platform=x64 /m

if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    goto :error
)

REM Create output directories
set "OUTPUT_DIR=build\Release"
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM Copy the executable
copy "!PROJECT_DIR!\x64\Release\software-renderer.exe" "!OUTPUT_DIR!"

if %ERRORLEVEL% NEQ 0 (
    echo Failed to copy executable.
    goto :error
)

echo.
echo =========================================
echo Build completed successfully!
echo.
echo You can now run the software renderer:
echo   build\Release\software-renderer.exe --generate-test-textures
echo   build\Release\software-renderer.exe --input examples\cube.obj --output output.tga
echo.

goto :end

:error
echo.
echo Build failed. Please check the error messages above.
exit /b 1

:end
exit /b 0 