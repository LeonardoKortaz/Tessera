@echo off
echo ====================================
echo    TESSERA - AUTOMATED BUILD
echo ====================================
echo.

if not exist "src\main.cpp" (
    echo ERROR: src\main.cpp not found
    echo Make sure you run this script from the Tessera project folder
    pause
    exit /b 1
)

if not exist "build" mkdir build
cd build

echo Configuring project...
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo Trying CMake without vcpkg...
    cmake ..
    if %ERRORLEVEL% NEQ 0 (
        echo.
        echo ERROR: CMake configuration failed!
        echo Check the error messages above for details.
        cd ..
        pause
        exit /b 1
    )
)

echo Building project...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    echo Check the compilation errors above for details.
    cd ..
    pause  
    exit /b 1
)

echo Copying SFML DLLs...
if exist "C:\vcpkg\installed\x64-windows\bin\sfml-audio-3.dll" (
    copy /Y "C:\vcpkg\installed\x64-windows\bin\sfml-*.dll" "bin\Release\" >nul 2>&1
    echo ✓ SFML DLLs copied successfully
) else (
    echo Warning: SFML DLLs not found in vcpkg, trying system path...
)

echo Copying assets...
if exist "..\Assets" (
    if exist "bin\Release\Assets" rmdir /s /q "bin\Release\Assets" >nul 2>&1
    xcopy /E /I /Q "..\Assets" "bin\Release\Assets" >nul 2>&1
    echo ✓ Assets copied successfully
)

cd ..
echo.
echo ====================================
echo    SUCCESS! BUILD COMPLETE
echo ====================================
echo Executable: build\bin\Release\Jigzter.exe
echo.
echo Run the game with: .\build\bin\Release\Jigzter.exe