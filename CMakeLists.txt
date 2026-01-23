cmake_minimum_required(VERSION 4.1)

# ============================================================
# Project
# ============================================================
project(microbit_baremetal LANGUAGES C CXX ASM)

# ============================================================
# User-configurable knobs (keep existing options)
# ============================================================
set(TARGET_NAME led CACHE STRING "Firmware target name")
set(ELF_TARGET "${TARGET_NAME}.elf")

option(BUILD_FIRMWARE "Build embedded firmware" ${CMAKE_CROSSCOMPILING})
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_TESTS_ARM_COMPILE_ONLY "Compile unit tests with ARM compiler (no link/run)" ON)

# ============================================================
# Common settings / flags
# ============================================================
# CPU / ABI flags for nRF52833 (micro:bit v2)
set(MB2_CPU_FLAGS
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
)

# Common warning flags (host + embedded)
set(COMMON_WARN_FLAGS
        -Wall
        -Wextra
)

# ============================================================
# Firmware (cross-compiled)
# ============================================================
if(BUILD_FIRMWARE)

    # ----------------------------
    # Sources
    # ----------------------------
    set(FW_SOURCES
            startup.s
            src/app/main.cpp
            src/app/queue.cpp
            src/app/irq_arm.c
            src/platform/nrf_gpio.cpp
            src/drivers/led_matrix.cpp
            src/drivers/buttons.cpp
    )

    add_executable(${ELF_TARGET} ${FW_SOURCES})

    # ----------------------------
    # Includes
    # ----------------------------
    target_include_directories(${ELF_TARGET} PRIVATE
            src/app/include
            src/platform/include
            src/drivers/include
    )

    # ----------------------------
    # Compile options
    # ----------------------------
    target_compile_options(${ELF_TARGET} PRIVATE
            ${MB2_CPU_FLAGS}

            # Debug vs Release
            $<$<CONFIG:Debug>:-Og -g3>
            $<$<CONFIG:Release>:-O2>

            -ffreestanding
            -fdata-sections
            -ffunction-sections

            ${COMMON_WARN_FLAGS}
    )

    # C++ only
    target_compile_options(${ELF_TARGET} PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>
    )

    # Release define
    target_compile_definitions(${ELF_TARGET} PRIVATE
            $<$<CONFIG:Release>:NDEBUG>
    )

    # ----------------------------
    # Link options
    # ----------------------------
    target_link_options(${ELF_TARGET} PRIVATE
            ${MB2_CPU_FLAGS}
            -nostdlib
            -Wl,--gc-sections
            -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
            -T ${CMAKE_SOURCE_DIR}/linker.ld
            $<$<CONFIG:Release>:-Wl,--strip-debug>
    )

    # ----------------------------
    # HEX generation
    # ----------------------------
    set(HEX_FILE "${CMAKE_BINARY_DIR}/${TARGET_NAME}.hex")

    # Keep your existing behavior: strip debug in Release during objcopy.
    # (Note: with multi-config generators, CMAKE_BUILD_TYPE may be empty;
    # leaving this as-is preserves your current intent without new options.)
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

    # ----------------------------
    # Deploy / Debug server helpers (pyOCD via Python module)
    # ----------------------------
    # Use Windows Python launcher so it works from PowerShell, Git Bash, MSYS2, and CLion
    find_program(PY_LAUNCHER py REQUIRED)

    add_custom_target(deploy
            COMMAND ${PY_LAUNCHER} -m pyocd flash --target nrf52833 --erase sector $<TARGET_FILE:${ELF_TARGET}>
            COMMAND ${PY_LAUNCHER} -m pyocd reset --target nrf52833
            DEPENDS ${ELF_TARGET}
            USES_TERMINAL
            VERBATIM
    )

    add_custom_target(debugserver
            COMMAND ${PY_LAUNCHER} -m pyocd gdbserver
            --target nrf52833
            --port 3333
            --telnet-port 4444
            --persist
            -O frequency=200000
            -O connect_mode=under-reset
            -O cmsis_dap.limit_packets=true
            USES_TERMINAL
            VERBATIM
    )
endif()

# ============================================================
# Unit tests (host build)
# ============================================================
if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)

    include(CTest)
    enable_testing()

    add_library(unity
            third_party/unity/src/unity.c
    )
    target_include_directories(unity PUBLIC
            third_party/unity/src
    )

    add_executable(test_queue
            tests/test_queue.cpp
            src/app/queue.cpp
            src/app/irq_host.c
    )

    # These headers live here in your repo tree:
    # src/app/include/{queue.h, irq.h, ...}
    target_include_directories(test_queue PRIVATE
            src/app/include
            third_party/unity/src
    )

    target_link_libraries(test_queue PRIVATE unity)
    target_compile_options(test_queue PRIVATE -g -O0 ${COMMON_WARN_FLAGS})

    add_test(NAME test_queue COMMAND test_queue)

endif()

# ============================================================
# Unit tests (ARM toolchain) - compile-only
# ============================================================
if(BUILD_TESTS_ARM_COMPILE_ONLY AND CMAKE_CROSSCOMPILING)

    add_library(unity_arm_obj OBJECT
            third_party/unity/src/unity.c
    )
    target_include_directories(unity_arm_obj PUBLIC
            third_party/unity/src
    )

    add_library(test_queue_arm_obj OBJECT
            tests/test_queue.cpp
    )
    target_include_directories(test_queue_arm_obj PRIVATE
            src/app/include
            third_party/unity/src
    )

    target_compile_options(unity_arm_obj PRIVATE
            ${MB2_CPU_FLAGS}
            -ffreestanding
            -fdata-sections -ffunction-sections
            ${COMMON_WARN_FLAGS}
    )

    target_compile_options(test_queue_arm_obj PRIVATE
            ${MB2_CPU_FLAGS}
            -ffreestanding
            -fdata-sections -ffunction-sections
            ${COMMON_WARN_FLAGS}
    )

    add_custom_target(arm_tests_compile
            DEPENDS unity_arm_obj test_queue_arm_obj
    )

endif()
