# Robot Starter Template (`apps/template_project`)

Production-ready modular starter template for incubating new autonomous embedded robotics prototypes on **STM32F446RE** (ARM Cortex-M4F @ 180 MHz).

---

## Intended Purpose

This template serves as the springboard for prototyping new experimental robotics platforms in `STM32-Projects`, such as:
- **Micromouse** (Maze exploration, Floodfill navigation, High-speed cornering)
- **Two-Wheel Balancing Robot** (Inverted pendulum, IMU angle estimation, Dual-loop PID/LQR)
- **Quadcopter / Drone Flight Controller** (Rate controllers, complementary/Kalman filtering, motor mixing)
- **Differential / Ackermann Mobile Rover** (Odometry, trajectory tracking, obstacle avoidance)

Once a robot reaches maturity (custom PCB designs, mechanical CAD, standalone firmware), it graduates out of `STM32-Projects` into its own dedicated repository (as demonstrated by [`STM32-Line-Follower-Robot`](https://github.com/ShubhamJain17r/STM32-Line-Follower-Robot)).

---

## Architecture & Features

1. **Deterministic 1 kHz Control Loop**:
   - Monotonic hardware timebase provided by `stm32::systick` with 1 ms tick resolution.
   - Jitter-free periodic control scheduling.

2. **Clean Lifecycle & State Machine**:
   - Standardized `RobotApp` interface:
     - `init()`: Safe hardware initialization and peripheral startup.
     - `update()`: Periodic 1 ms update cycle.
     - `State`: `INIT`, `IDLE`, `CALIBRATING`, `RUNNING`, `ERROR`.

3. **Zero-Overhead Bare-Metal C++17**:
   - Zero vendor HAL bloat, zero dynamic memory allocation (heap = 0 bytes).
   - Consumes modern namespaced targets: `stm32::hal`, `stm32::components`, and `stm32::algorithms`.

4. **Hardware Pinout Isolation**:
   - All GPIOs, PWM timer channels, encoder timer instances, and ADC channels are cleanly decoupled in [`include/robot_config.hpp`](include/robot_config.hpp).

---

## Quick Start: Prototyping a New Robot

1. **Duplicate this directory**:
   ```bash
   cp -r apps/template_project apps/my_new_robot
   ```
2. **Update Target Name**:
   - In `apps/my_new_robot/CMakeLists.txt`: `set(APP_TARGET my_new_robot)`
   - In `apps/my_new_robot/Makefile`: `TARGET = my_new_robot`
3. **Configure Hardware**:
   - Edit `include/robot_config.hpp` with your custom pin mappings and peripheral channels.
4. **Build & Flash**:
   ```bash
   # From root:
   make APP=apps/my_new_robot
   make APP=apps/my_new_robot flash
   ```
