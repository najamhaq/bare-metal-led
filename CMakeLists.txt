cmake_minimum_required(VERSION 3.20)
project(microbit_baremetal LANGUAGES C CXX ASM)

set(TARGET_NAME led CACHE STRING "Firmware target name")
set(ELF_TARGET "${TARGET_NAME}.elf")

add_executable(${ELF_TARGET}
  startup.s
  main.cpp
)

set(CPU_FLAGS
  -mcpu=cortex-m4
  -mthumb
  -mfpu=fpv4-sp-d16
  -mfloat-abi=hard
)

target_compile_options(${ELF_TARGET} PRIVATE
  ${CPU_FLAGS}
  $<$<CONFIG:Debug>:-Og -g3>
  $<$<CONFIG:Release>:-O2>
  -ffreestanding
  -fdata-sections -ffunction-sections
  -Wall -Wextra
)

target_compile_options(${ELF_TARGET} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
)

target_link_options(${ELF_TARGET} PRIVATE
  ${CPU_FLAGS}
  -nostdlib
  -Wl,--gc-sections
  -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
  -T ${CMAKE_SOURCE_DIR}/linker.ld
)

# Hex name derived from TARGET_NAME (simple and reliable)
set(HEX_FILE "${CMAKE_BINARY_DIR}/${TARGET_NAME}.hex")

add_custom_command(
  OUTPUT "${HEX_FILE}"
  COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${ELF_TARGET}> "${HEX_FILE}"
  DEPENDS ${ELF_TARGET}
  VERBATIM
)

add_custom_target(hex DEPENDS "${HEX_FILE}")

add_custom_target(flash
  COMMAND powershell -NoProfile -ExecutionPolicy Bypass
          -File "${CMAKE_SOURCE_DIR}/flash.ps1"
          -HexPath "${HEX_FILE}"
  DEPENDS hex
  VERBATIM
)
