@echo off
REM Windows build script for DIT-Transfer-Tools using CMake and MSVC
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
cd ..
pause