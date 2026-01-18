cmake_minimum_required(VERSION 4.1)
project(microbit_baremetal LANGUAGES C CXX ASM)

set(TARGET_NAME led CACHE STRING "Firmware target name")
set(ELF_TARGET "${TARGET_NAME}.elf")

option(BUILD_FIRMWARE "Build embedded firmware" ${CMAKE_CROSSCOMPILING})
option(BUILD_TESTS "Build unit tests" ON)


#--------------------
# Build Firmware
# ---------------------

if(BUILD_FIRMWARE)
    # CPU / ABI flags for nRF52833 (micro:bit v2)
    set(CPU_FLAGS
            -mcpu=cortex-m4
            -mthumb
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
    )

    add_executable(${ELF_TARGET}
            startup.s
            main.cpp
            include/message.h
            src/message.cpp
            include/queue.h
            src/queue.cpp
            include/irq.h
            src/irq_arm.c
    )
    target_include_directories(${ELF_TARGET} PRIVATE
            include
    )

    # CPU / ABI flags for nRF52833 (micro:bit v2)
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

    target_compile_definitions(${ELF_TARGET} PRIVATE
            $<$<CONFIG:Release>:NDEBUG>
    )

    target_link_options(${ELF_TARGET} PRIVATE
            ${CPU_FLAGS}
            -nostdlib
            -Wl,--gc-sections
            -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map
            -T ${CMAKE_SOURCE_DIR}/linker.ld
            $<$<CONFIG:Release>:-Wl,--strip-debug>
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

    # ------ Setup debug  server ------
    add_custom_target(debugserver
            COMMAND pyocd gdbserver
            --target nrf52833
            --port 3333
            --telnet-port 4444
            --persist
            -O frequency=200000
            -O connect_mode=under-reset
            -O cmsis_dap.limit_packets=true
            VERBATIM
            USES_TERMINAL
    )


endif()



# ----------------------------
# Unit tests (host)
# ----------------------------
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
            src/queue.cpp
            include/irq.h
            src/irq_host..c
    )
    target_include_directories(test_queue PRIVATE
            include
    )
    target_link_libraries(test_queue PRIVATE unity)
    target_compile_options(test_queue PRIVATE -g -O0)
    add_test(NAME test_queue COMMAND test_queue)

endif()


# ----------------------------
# Unit tests (ARM toolchain) - compile-only
# ----------------------------
option(BUILD_TESTS_ARM_COMPILE_ONLY "Compile unit tests with ARM compiler (no link/run)" ON)

if(BUILD_TESTS_ARM_COMPILE_ONLY AND CMAKE_CROSSCOMPILING)

    # Reuse/define CPU_FLAGS for ARM compiles (ensure this exists in this scope)
    set(CPU_FLAGS
            -mcpu=cortex-m4
            -mthumb
            -mfpu=fpv4-sp-d16
            -mfloat-abi=hard
    )

    # Unity compiled as objects (no link)
    add_library(unity_arm_obj OBJECT
            third_party/unity/src/unity.c
    )
    target_include_directories(unity_arm_obj PUBLIC
            third_party/unity/src
    )

    # Test sources compiled as objects (no link)
    add_library(test_add_arm_obj OBJECT
            tests/test_queue.cpp
    )
    target_include_directories(test_add_arm_obj PRIVATE
            include
            third_party/unity/src
    )

    # Apply ARM-ish compile flags to both object libs
    target_compile_options(unity_arm_obj PRIVATE
            ${CPU_FLAGS}
            -ffreestanding
            -fdata-sections -ffunction-sections
            -Wall -Wextra
    )

    target_compile_options(test_add_arm_obj PRIVATE
            ${CPU_FLAGS}
            -ffreestanding
            -fdata-sections -ffunction-sections
            -Wall -Wextra
    )

    # Convenience target so you can run: cmake --build build-arm --target arm_tests_compile
    add_custom_target(arm_tests_compile
            DEPENDS unity_arm_obj test_add_arm_obj
    )

endif()
