# High-Speed Closed-Loop Line Follower Benchmark (`apps/line_follower_benchmark`)

Target MCU: **STM32F446RE** (ARM Cortex-M4F @ 180 MHz, 512 KB Flash, 128 KB SRAM)

This application serves as the **primary golden benchmark reference** for the `STM32-Projects` incubator. It demonstrates a complete, production-grade closed-loop robotics system utilizing all layers of our embedded robotics stack:
- **`lib/STM32-Library`** (`stm32::hal`): Bare-metal peripherals (SysTick, Timers, PWM, ADC + DMA, UART, GPIO).
- **`lib/STM32-Components`** (`stm32::components`): TB6612FNG H-bridge, Quadrature Encoders, QTR sensor array, HC-05 Bluetooth, Active Buzzer.
- **`lib/Embedded-Algorithms`** (`stm32::algorithms`): Digital filtering, math utilities, and closed-loop control.

---

## Key Subsystems & Architecture

```
┌────────────────────────────────────────────────────────────────────────┐
│                   1 kHz Deterministic Control Loop                     │
│                                                                        │
│   ┌───────────────┐     ┌────────────────┐     ┌───────────────────┐   │
│   │  8-Ch Optical │     │ Silicon Timer  │     │ 3S LiPo Battery   │   │
│   │   QTR Array   │     │ Encoders (TIM) │     │ Voltage Monitor   │   │
│   └───────┬───────┘     └───────┬────────┘     └─────────┬─────────┘   │
│           │                     │                        │             │
│           ▼                     ▼                        ▼             │
│   ┌───────────────┐     ┌────────────────┐     ┌───────────────────┐   │
│   │ Line Position │     │ Odometry & RPM │     │ Safety Protection │   │
│   │ Error (PID)   │     │ Speed Estimator│     │ & Buzzer Alarms   │   │
│   └───────┬───────┘     └───────┬────────┘     └───────────────────┘   │
│           │                     │                                      │
│           └──────────┬──────────┘                                      │
│                      │                                                 │
│                      ▼                                                 │
│           ┌──────────────────────┐                                     │
│           │ Dynamic Curvature    │                                     │
│           │ Speed Scheduler      │                                     │
│           └──────────┬───────────┘                                     │
│                      │                                                 │
│                      ▼                                                 │
│           ┌──────────────────────┐     ┌───────────────────────────┐   │
│           │ 20 kHz Ultrasonic    │     │ 115200 Baud Wireless HC05 │   │
│           │ TB6612 Motor Drive   │◄───►│ Live Telemetry & Tuning   │   │
│           └──────────────────────┘     └───────────────────────────┘   │
└────────────────────────────────────────────────────────────────────────┘
```

1. **Deterministic 1 kHz Closed-Loop Execution**:
   - Driven by hardware SysTick monotonic timebase at 1.0 ms intervals.
   - Zero jitter, zero dynamic memory allocation (0 bytes heap).

2. **20 kHz Ultrasonic Motor Drive**:
   - `TIM2` hardware PWM generation running at 20 kHz (eliminates audible motor coil whine).
   - Atomic BSRR direction control with slew-rate limiting and motor deadband compensation.

3. **Silicon-Decoded Quadrature Encoder Odometry**:
   - Direct hardware quadrature decoding via `TIM3` (Motor A) and `TIM4` (Motor B).
   - Independent wheel velocity and distance tracking.

4. **Circular DMA Optical Sensor Acquisition & Ambient Rejection**:
   - ADC1 multichannel circular scan mode via DMA for 8 reflectance sensor channels.
   - Synchronized emitter modulation via MOSFET gate for active ambient light rejection.

5. **Wireless Telemetry & Live Parameter Tuning**:
   - 115200 baud UART streaming via HC-05 Bluetooth module.
   - Runtime tuning commands (`<KP:...>`, `<KD:...>`, `<SPEED:...>`).

6. **3S LiPo Battery Protection**:
   - Voltage divider sensing on ADC1 channel 4.
   - Multi-threshold acoustic alarms via active buzzer.

---

## Hardware Pinout Reference

| Function | Pin | Peripheral Channel | Description |
| :--- | :--- | :--- | :--- |
| **Status Heartbeat LED** | `PA5` | GPIO Output | Nucleo Onboard Green LED LD2 |
| **User Pushbutton** | `PC13` | GPIO Input (Pull-up) | Nucleo User Button B1 |
| **Motor A PWM (Left)** | `PA0` | `TIM2_CH1` (AF1) | 20 kHz Ultrasonic PWM |
| **Motor A IN1 / IN2** | `PC8`, `PC9` | GPIO Output | Motor A Direction Control |
| **Motor B PWM (Right)** | `PA1` | `TIM2_CH2` (AF1) | 20 kHz Ultrasonic PWM |
| **Motor B IN1 / IN2** | `PC11`, `PC12`| GPIO Output | Motor B Direction Control |
| **Encoder A (Left)** | `PA6`, `PA7` | `TIM3_CH1`, `TIM3_CH2` (AF2) | Silicon Quadrature Decoder |
| **Encoder B (Right)** | `PB6`, `PB7` | `TIM4_CH1`, `TIM4_CH2` (AF2) | Silicon Quadrature Decoder |
| **QTR Reflectance (8x)**| `PC0-PC5`, `PB0-PB1` | `ADC1_IN8-IN15` | Circular Scan DMA |
| **Emitter MOSFET Gate** | `PB14` | GPIO Output | Active Ambient Light Rejection |
| **Battery Voltage Sense**| `PA4` | `ADC1_IN4` | 3S LiPo Divider Input |
| **Active Buzzer** | `PC6` | GPIO Output | Acoustic Alarm & Audio Feedback |
| **Telemetry UART** | `PA2` (TX), `PA3` (RX) | `USART2` | 115200 Baud HC-05 Bluetooth |

---

## Building & Flashing

From the repository root:

```bash
# Build binary
make APP=line_follower_benchmark

# Check binary footprint
make APP=line_follower_benchmark size

# Flash via OpenOCD
make APP=line_follower_benchmark flash

# Flash via ST-Link
make APP=line_follower_benchmark flash-stlink
```

Or build via modern CMake:

```bash
cmake -B build -S .
cmake --build build --target line_follower_benchmark.elf
```
