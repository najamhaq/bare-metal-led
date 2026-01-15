cmake_minimum_required(VERSION 3.20)
project(microbit_baremetal LANGUAGES C CXX ASM)

set(TARGET_NAME led CACHE STRING "Firmware target name")
set(ELF_TARGET "${TARGET_NAME}.elf")

add_executable(${ELF_TARGET}
        startup.s
        main.cpp
)

# CPU / ABI flags for nRF52833 (micro:bit v2)
set(CPU_FLAGS
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

# Output artifacts go to the active profile build dir (Debug-ARM vs Release-ARM)
set(HEX_FILE "${CMAKE_BINARY_DIR}/${TARGET_NAME}.hex")

# ---- Compile flags ----
target_compile_options(${ELF_TARGET} PRIVATE
        ${CPU_FLAGS}

        # Debug vs Release
        $<$<CONFIG:Debug>:-Og -g3>
        $<$<CONFIG:Release>:-O2>

        -ffreestanding
        -fdata-sections -ffunction-sections
        -Wall -Wextra
)

# C++ only
target_compile_options(${ELF_TARGET} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
)

# Release define
target_compile_definitions(${ELF_TARGET} PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
)

# ---- Link flags ----
target_link_options(${ELF_TARGET} PRIVATE
        ${CPU_FLAGS}
        -nostdlib
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
        -T ${CMAKE_SOURCE_DIR}/linker.ld

        # IMPORTANT: remove debug symbols from the *Release ELF* itself
        $<$<CONFIG:Release>:-Wl,--strip-debug>
)

# ---- HEX generation ----
# Avoid generator-expression empty-arg bug by deciding at configure time
set(OBJCOPY_EXTRA_ARGS "")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OBJCOPY_EXTRA_ARGS --strip-debug)
endif()

add_custom_command(
        OUTPUT "${HEX_FILE}"
        COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_EXTRA_ARGS} -O ihex
        $<TARGET_FILE:${ELF_TARGET}> "${HEX_FILE}"
        DEPENDS ${ELF_TARGET}
        COMMAND_EXPAND_LISTS
        VERBATIM
)

add_custom_target(hex DEPENDS "${HEX_FILE}")

# ---- deploy using (pyflash) ----
add_custom_target(deploy
        COMMAND pyocd flash --target nrf52833 --erase sector $<TARGET_FILE:${ELF_TARGET}>
        COMMAND pyocd reset --target nrf52833
        DEPENDS ${ELF_TARGET}
        VERBATIM
)
