@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

set PATH=C:\Qt\6.11.0\mingw_64\bin;%PATH%

set EXE=release\BlinkDateConverter.exe
set DIST=dist\BlinkDateConverter

if not exist %EXE% (
    echo ERROR: %EXE% not found.
    echo Run build.bat first.
    pause
    exit /b 1
)

:: Check windeployqt
where windeployqt >nul 2>&1
if errorlevel 1 (
    echo ERROR: windeployqt not found. Make sure Qt is in PATH.
    pause
    exit /b 1
)

:: Clean old dist
if exist dist rmdir /s /q dist

:: Create dist folder
mkdir %DIST%

:: Copy executable
copy %EXE% %DIST%\ >nul

:: Deploy Qt DLLs
echo Running windeployqt...
windeployqt %DIST%\BlinkDateConverter.exe --no-translations --no-system-d3d-compiler --no-compiler-runtime --no-opengl-sw
if errorlevel 1 (
    echo ERROR: windeployqt failed.
    pause
    exit /b 1
)

:: Create ZIP
echo Creating ZIP archive...
if exist %DIST%.zip del %DIST%.zip
powershell -Command "Compress-Archive -Path '%DIST%\*' -DestinationPath '%DIST%.zip'"

echo.
echo ========================================
echo Deployment completed successfully.
echo.
echo Folder: %DIST%\
echo ZIP:    %DIST%.zip
echo ========================================
pause
