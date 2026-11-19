# Shared Embedded Robotics Libraries (`lib/`)

This directory contains the foundational, modular shared libraries consumed across all application prototypes in `STM32-Projects`.

---

## Architecture Overview

```
lib/
├── STM32-Library/              # [Layer 1] Bare-Metal C++17 Hardware Abstraction Layer
│   ├── Inc/stm32/              # Low-level peripheral drivers (GPIO, Timers, ADC, DMA, SysTick, UART, SPI, I2C)
│   ├── Drivers/CMSIS/          # Official ARM CMSIS and ST STM32F4xx device headers
│   └── Startup/                # Cortex-M4F startup assembly & linker scripts
│
├── STM32-Components/           # [Layer 2] External Sensor & Actuator Device Drivers
│   └── include/components/     # Hardware drivers (TB6612FNG, QTR Reflectance, HC-05, Encoders, Buzzers)
│
├── Embedded-Algorithms/        # [Algorithms] Platform-Independent Robotics Algorithms
│   └── include/algorithms/     # Control (PID), Navigation (Floodfill, LSRB), Kinematics, Filters
│
└── CMakeLists.txt              # Exposes stm32::hal, stm32::components, and stm32::algorithms
```

---

## Modern CMake Targets

Each library layer is exposed as a namespaced, modern CMake target configured in [`lib/CMakeLists.txt`](CMakeLists.txt):

| Target | Type | Provides | Dependencies |
| :--- | :--- | :--- | :--- |
| **`stm32::hal`** | `STATIC` | Peripheral drivers, CMSIS includes, startup code, linker script, specs (`nano.specs`, `nosys.specs`), hardware definitions (`-DSTM32F446xx`). | None |
| **`stm32::components`** | `INTERFACE` | Device driver headers (`include/components/*`). | `stm32::hal` |
| **`stm32::algorithms`** | `INTERFACE` | Algorithm headers (`include/algorithms/*`), math utils, filters. | None (Platform-agnostic) |

### Consuming in an Application
Any application under `apps/` consumes these targets with a single line in its `CMakeLists.txt`:

```cmake
target_link_libraries(${APP_TARGET}.elf PRIVATE
    stm32::components
    stm32::algorithms
)
```

All MCU compilation flags, includes, definitions, and linker scripts propagate transitively.

---

## Submodule Management

These libraries are tracked as standard Git submodules pointing to their dedicated repositories:

```bash
# Update all submodules to their configured revisions
git submodule update --init --recursive

# Pull latest commits on submodule tracking branches
git submodule update --remote --merge
```
