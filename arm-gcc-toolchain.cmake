# arm-gcc-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_DIR "C:/arm/bin")

set(CMAKE_C_COMPILER   "${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe"  CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}/arm-none-eabi-g++.exe"  CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe"  CACHE FILEPATH "" FORCE)

set(CMAKE_OBJCOPY      "${TOOLCHAIN_DIR}/arm-none-eabi-objcopy.exe" CACHE FILEPATH "" FORCE)
set(CMAKE_SIZE         "${TOOLCHAIN_DIR}/arm-none-eabi-size.exe"    CACHE FILEPATH "" FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
