@echo off
REM Windows Build Script for DIT-Transfer-Tools

if "%1"=="clean" (
    rmdir /s /q build
    goto end
)

if not exist build mkdir build
cd build

REM Assume Qt6 is installed and qmake is in PATH
qmake ..\DIT.pro

if %errorlevel% neq 0 (
    echo qmake failed
    exit /b 1
)

nmake

if %errorlevel% neq 0 (
    echo nmake failed
    exit /b 1
)

echo Build successful
goto end

:end