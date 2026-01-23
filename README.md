# Bare-Metal ARM Cortex-M Firmware (micro:bit V2 / nRF52833)

## Overview

This repository contains a **bare-metal firmware project** targeting the **BBC micro:bit V2**, built directly for the **Nordic nRF52833 (ARM Cortex-M4F)** microcontroller.

The objective of this project is **not** to rely on vendor frameworks or runtime libraries, but to rebuild a precise and modern embedded-systems mental model by working directly with:

- Memory-mapped GPIO registers
- Custom startup code and linker scripts
- Interrupt-driven timing (incrementally)
- Hardware-level LED matrix control

Though no high-level abstraction is used here, the code is structured and commented for clarity and maintainability.

---

## Hardware

- **Board:** BBC micro:bit V2 (development board only)
- **Microcontroller:** Nordic nRF52833
- **CPU Core:** ARM Cortex-M4F (ARMv7E-M)
- **Programming Method:** DAPLink / CMSIS-DAP (via USB)

> The micro:bit is used strictly as a **carrier board**.  
> All firmware targets the **nRF52833 MCU directly**.

---

## Design Philosophy

### Bare-Metal by Intent

This project intentionally avoids:

- CODAL / micro:bit runtime
- mbed
- CMSIS HAL drivers
- Arduino-style APIs
- RTOS (for now)

Instead, it includes:

- Custom `startup.s` (vector table + reset handler)
- Custom `linker.ld`
- Freestanding C/C++ (`-ffreestanding -nostdlib`)
- Direct peripheral access via memory-mapped registers

This ensures full visibility into:

- Reset and boot flow
- Stack initialization
- Exception and interrupt handling
- Peripheral side effects

---

## LED Matrix Control

The micro:bit’s 5×5 display is a **multiplexed LED matrix**, not 25 independent LEDs.

### Electrical Model

- **Rows (anodes):** driven HIGH
- **Columns (cathodes):** driven LOW
- LEDs illuminate at **ROW × COLUMN intersections**
- Only one row is active at a time during scanning

### GPIO Reality

Row and column pins are **not sequential GPIO numbers**. They are wired based on PCB routing constraints, not logical LED layout.

Example mappings:

- **Rows:** `P0.15, P0.19, P0.21, P0.22, P0.24`
- **Columns:** `P0.11, P0.28, P0.30, P0.31, P1.05`

A logical `(row, column)` abstraction is mapped explicitly to physical GPIO pins.

---

## GPIO Access Model

The nRF52833 uses **memory-mapped GPIO**.

Each port exposes:

- `OUT` — absolute output latch
- `OUTSET` — write-1-to-set
- `OUTCLR` — write-1-to-clear
- `DIRSET` — configure pins as outputs
- `DIRCLR` — configure pins as inputs

This project uses:

- `OUT` for deterministic initialization
- `OUTSET / OUTCLR` for atomic runtime control

No software shadow registers are required.

---

## Timing and Animation

Initial bring-up uses short busy-wait delays for validation only.

Planned / implemented concepts include:

- SysTick or hardware TIMER interrupts
- Blocking LED animations
- Interrupt-driven button handling
- Time-based debounce logic

---

## Project Structure

> Note: this README intentionally reflects the **current repo**. There are no standalone `flash.ps1` / `clean.cmd` scripts; flashing and cleaning are handled via CMake targets.

```
mb2-baremetal/
├── CMakeLists.txt
├── arm-gcc-toolchain.cmake
├── linker.ld
├── startup.s
│
├── src/
│   ├── app/
│   │   ├── include/
│   │   ├── main.cpp
│   │   ├── message.cpp
│   │   ├── queue.cpp
│   │   └── irq_arm.c
│   │
│   ├── platform/
│   │   ├── include/
│   │   └── nrf_gpio.cpp
│   │
│   └── drivers/
│       ├── include/
│       ├── led_matrix.cpp
│       └── buttons.cpp
│
├── tests/
│   └── test_queue.cpp
│
├── third_party/
│   └── unity/
│
├── build/        (generated, not versioned)
└── README.md
```

---

## Build, Flash, and Debug (CMake)

### Prerequisites

- **CMake**
- **Ninja** (recommended; Multi-Config works well with CLion)
- **GNU Arm Embedded Toolchain** (`arm-none-eabi-gcc`, `arm-none-eabi-g++`, `objcopy`, `gdb`)
- **pyOCD** (for flashing + gdbserver)

### 1) Configure (one-time)

From the repo root:

```bash
cmake -S . -B build -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE=arm-gcc-toolchain.cmake
```

This creates a single `build/` folder containing both configurations:
- `build/Debug/`
- `build/Release/`

### 2) Build firmware

Debug:
```bash
cmake --build build --config Debug --target led.elf
```

Release:
```bash
cmake --build build --config Release --target led.elf
```

### 3) Generate HEX

Debug:
```bash
cmake --build build --config Debug --target hex
```

Release:
```bash
cmake --build build --config Release --target hex
```

Output:
- `build/led.hex`

### 4) Flash to micro:bit (pyOCD)

Debug:
```bash
cmake --build build --config Debug --target deploy
```

Release:
```bash
cmake --build build --config Release --target deploy
```

### 5) Start a GDB debug server (pyOCD)

```bash
cmake --build build --config Debug --target debugserver
```

In another terminal, connect with GDB:

```bash
arm-none-eabi-gdb build/Debug/led.elf
```

Inside GDB:

```gdb
target remote 127.0.0.1:3333
monitor reset halt
load
continue
```

---

## Host Unit Tests (Unity)

Configure a native build (no toolchain file):

```bash
cmake -S . -B build-host -G Ninja
```

Build + run tests:

```bash
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

---

## What This Project Demonstrates

- Clear separation of ARM CPU vs MCU peripherals
- Correct use of memory-mapped I/O
- Deterministic GPIO initialization
- LED matrix multiplexing
- Timer-driven state machines
- Button input with debounce
- Button-controlled animation
- Queue-based event handling
- Unit testing of queue implementation

---

## Shortcomings

We are well aware of the following limitations in the current implementation:

- No RTOS or advanced scheduling yet
- Basic error handling and edge cases not fully covered
- Polling of button states can miss presses during LED updates
- No power management or low-power modes implemented

---

## Next Steps

Planned extensions:

- Full 5×5 LED scan loop using timer interrupts
- Button-controlled animation pause/resume
- SysTick vs hardware TIMER comparison
- Optional RTOS introduction after fundamentals

---

## License

Educational and demonstrational use only.
