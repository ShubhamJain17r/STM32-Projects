# STM32-Projects: Incubator & Multi-Robot Template Playground

Production-grade embedded robotics incubator and multi-robot template playground targeting the **STM32F446RE** microcontroller (ARM Cortex-M4F @ 180 MHz).

This repository serves as the **incubator playground** where new experimental robot prototypes (e.g., Micromouse, Two-Wheel Balancing Robot, Drone Flight Controller, Autonomous Rover) begin their development lifecycle, mature, and graduate into dedicated standalone product repositories.

---

## Graduation Model

Robotics projects at our lab follow a three-stage graduation lifecycle:

1. **Incubator Prototyping (`STM32-Projects/apps/`)**:
   - New robot ideas start inside `apps/` using [`apps/template_project`](apps/template_project/README.md) as the starter template.
   - Algorithms, sensor drivers, and closed-loop control schemes are validated on NUCLEO-F446RE breadboard/protoboard testbenches.
2. **Standardization & Architecture Maturation**:
   - Hardware pin mappings, timer allocations, and communications are standardized against [`lib/STM32-Library`](lib/README.md), [`lib/STM32-Components`](lib/README.md), and [`lib/Embedded-Algorithms`](lib/README.md).
3. **Official Graduation (Dedicated Product Repository)**:
   - Once a robot reaches full hardware-software maturity, it graduates into its own dedicated repository complete with standalone firmware, KiCad schematic/layout PCBs, and mechanical CAD.
   - **Graduated Reference**: [`STM32-Line-Follower-Robot`](https://github.com/ShubhamJain17r/STM32-Line-Follower-Robot) (Officially graduated from this incubator).

---

## Repository Structure

```
STM32-Projects/
├── apps/
│   ├── line_follower_benchmark/    # [Benchmark] Clean reference benchmark for 1 kHz closed-loop control
│   └── template_project/           # [Starter Template] Starter foundation for future robots (e.g. Micromouse)
│
├── lib/
│   ├── STM32-Library/              # [Layer 1] Bare-metal C++17 Peripheral Engine (stm32::hal)
│   ├── STM32-Components/           # [Layer 2] External Sensor & Actuator Drivers (stm32::components)
│   ├── Embedded-Algorithms/        # [Algorithms] Shared Robotics Algorithms (stm32::algorithms)
│   └── README.md                   # Shared libraries architecture & target documentation
│
├── CMakeLists.txt                  # Dynamic application discovery & modern CMake orchestrator
├── Makefile                        # Unified root Makefile runner with test suite
└── README.md                       # Documentation and incubator index
```

---

## Applications Index

| Application | Target MCU | Key Features / Hardware | Role | Documentation |
| :--- | :--- | :--- | :--- | :--- |
| **`apps/line_follower_benchmark`** | STM32F446RE | 1 kHz closed loop, dual encoder odometry, 20 kHz TB6612 PWM, QTR reflectance array, HC-05 telemetry | Primary Benchmark Reference | [`README.md`](apps/line_follower_benchmark/README.md) |
| **`apps/template_project`** | STM32F446RE | Deterministic 1 ms SysTick control loop, RobotApp lifecycle state machine, configurable pinout | Starter Template for New Robots | [`README.md`](apps/template_project/README.md) |

---

## Shared Libraries Index (`lib/`)

Applications consume shared components via modern, namespaced CMake targets defined in [`lib/CMakeLists.txt`](lib/CMakeLists.txt) (detailed in [`lib/README.md`](lib/README.md)):

| Target | Layer | Description | Key Modules |
| :--- | :--- | :--- | :--- |
| **`stm32::hal`** | Layer 1 | Bare-metal C++17 Peripheral Engine & CMSIS | GPIO, ADC, Timers, PWM, UART, SPI, I2C, DMA, SysTick |
| **`stm32::components`** | Layer 2 | External Sensor & Actuator Device Drivers | TB6612FNG, QTR-8A, HC-05 Bluetooth, Encoders, Buzzers |
| **`stm32::algorithms`** | Algorithms | Platform-Agnostic Robotics Algorithms | PID Controller, Floodfill, LSRB, Kinematics, Low-Pass Filter |

---

## Build Instructions

### Prerequisites
- GNU ARM Embedded Toolchain (`arm-none-eabi-gcc` / `g++` 7.2.1+)
- Host C++17 Compiler (`g++` / `clang++`) & CMake 3.16+ (for test suite)
- GNU Make 4.0+
- OpenOCD or `st-flash` for flashing to NUCLEO-F446RE

---

### Building via Makefile

```bash
# 1. Build default benchmark application (apps/line_follower_benchmark)
make

# 2. Build a specific application prototype
make APP=apps/template_project
# or by shorthand:
make APP=template_project

# 3. Clean build artifacts
make clean                  # Clean current APP
make clean-all              # Clean all apps in apps/*

# 4. Check binary memory footprint
make size

# 5. Flash to Nucleo board
make flash                  # Flash via OpenOCD
make flash-stlink           # Flash via ST-Link utility
```

---

### Automated Testing & Submodule Verification

A unified test suite verifies integrity across all layers:

```bash
# Run full test suite (Layer 1 HAL, Layer 2 Components, Algorithms Unit Tests, and Apps)
make test

# Run only submodule compilation checks (Layer 1 & Layer 2)
make test-submodules

# Run only Embedded-Algorithms unit tests (via CTest)
make test-algorithms
```

---

### Building via Modern CMake

The root `CMakeLists.txt` automatically discovers all applications under `apps/` and configures them with modern CMake targets (`stm32::hal`, `stm32::components`, `stm32::algorithms`):

```bash
# Configure and build all applications
mkdir -p build && cd build
cmake ..
cmake --build .

# Flash specific application via CMake target
cmake --build . --target line_follower_benchmark
```

---

## Starting a New Robot Prototype

To incubate a new robot (e.g., `micromouse_prototype`):

```bash
# 1. Copy the starter template
cp -r apps/template_project apps/micromouse_prototype

# 2. Update APP_TARGET in apps/micromouse_prototype/CMakeLists.txt and TARGET in Makefile
sed -i 's/template_project/micromouse_prototype/g' apps/micromouse_prototype/CMakeLists.txt
sed -i 's/template_project/micromouse_prototype/g' apps/micromouse_prototype/Makefile

# 3. Customize hardware pins in include/robot_config.hpp
# 4. Build immediately from root
make APP=micromouse_prototype
```
