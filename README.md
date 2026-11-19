# STM32-Projects: Incubator & Multi-Robot Template Playground

Production-grade embedded robotics incubator and multi-robot template playground targeting the **STM32F446RE** microcontroller (ARM Cortex-M4F @ 180 MHz).

This repository serves as the **incubator playground** where new experimental robot prototypes (e.g., Micromouse, Two-Wheel Balancing Robot, Drone Flight Controller, Autonomous Rover) begin their development lifecycle, mature, and graduate into dedicated standalone product repositories.

---

## Graduation Model

Robotics projects at our lab follow a three-stage graduation lifecycle:

1. **Incubator Prototyping (`STM32-Projects/apps/`)**:
   - New robot ideas start inside `apps/` using `apps/template_project` as the starter template.
   - Algorithms, sensor drivers, and closed-loop control schemes are validated on NUCLEO-F446RE breadboard/protoboard testbenches.
2. **Standardization & Architecture Maturation**:
   - Hardware pin mappings, timer allocations, and communications are standardized against `lib/STM32-Library`, `lib/STM32-Components`, and `lib/Embedded-Algorithms`.
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
│   └── Embedded-Algorithms/        # [Algorithms] Shared Robotics Algorithms (stm32::algorithms)
│
├── CMakeLists.txt                  # Dynamic application discovery & modern CMake orchestrator
├── Makefile                        # Unified root Makefile runner
└── README.md                       # Documentation and incubator index
```

---

## Applications Index

| Application | Target MCU | Key Features / Hardware | Role |
| :--- | :--- | :--- | :--- |
| **[`apps/line_follower_benchmark`](apps/line_follower_benchmark)** | STM32F446RE | 1 kHz closed-loop loop, dual encoder odometry, 20 kHz TB6612 PWM, QTR reflectance array, HC-05 telemetry | Primary Benchmark Reference |
| **[`apps/template_project`](apps/template_project)** | STM32F446RE | Deterministic 1 ms SysTick control loop, RobotApp lifecycle state machine, configurable pinout | Starter Template for New Robots |

---

## Build Instructions

### Prerequisites
- GNU ARM Embedded Toolchain (`arm-none-eabi-gcc` / `g++` 7.2.1+)
- GNU Make 4.0+ or CMake 3.16+
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

# 6. Validate linked submodules
make test-submodules
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
