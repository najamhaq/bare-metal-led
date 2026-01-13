@echo off
setlocal

set "TC=%~dp0arm-gcc-toolchain.cmake"
set "NINJA=C:\tools\ninja\ninja.exe"

cmake -S "%~dp0" -B "%~dp0build" -G Ninja -DCMAKE_MAKE_PROGRAM=%NINJA% "-DCMAKE_TOOLCHAIN_FILE=%TC%"
cmake --build "%~dp0build" --target flash

endlocal
