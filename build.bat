@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

set PATH=C:\Qt\6.11.0\mingw_64\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%

:: Check tools
where qmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: qmake not found. Make sure Qt is installed and in PATH.
    pause
    exit /b 1
)

where mingw32-make >nul 2>&1
if errorlevel 1 (
    echo ERROR: mingw32-make not found. Make sure MinGW is in PATH.
    pause
    exit /b 1
)

:: Handle arguments
set MODE=%~1
if "%MODE%"=="" set MODE=release

if /I "%MODE%"=="clean" (
    echo Cleaning build artifacts...
    if exist Makefile mingw32-make clean
    if exist Makefile del Makefile
    if exist Makefile.Debug del Makefile.Debug
    if exist Makefile.Release del Makefile.Release
    if exist .qmake.stash del .qmake.stash
    if exist build rmdir /s /q build
    if exist release rmdir /s /q release
    echo Clean done.
    pause
    exit /b 0
)

if /I "%MODE%"=="rebuild" (
    echo Rebuilding from scratch...
    if exist Makefile mingw32-make clean
    if exist Makefile del Makefile
    if exist Makefile.Debug del Makefile.Debug
    if exist Makefile.Release del Makefile.Release
    if exist .qmake.stash del .qmake.stash
    if exist build rmdir /s /q build
    if exist release rmdir /s /q release
)

:: Generate Makefile
echo Generating Makefile...
qmake src\BlinkDateConverter.pro
if errorlevel 1 (
    echo ERROR: qmake failed.
    pause
    exit /b 1
)

:: Build
echo Building release...
mingw32-make -j4
if errorlevel 1 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully.
echo Output: release\BlinkDateConverter.exe
echo ========================================
pause
