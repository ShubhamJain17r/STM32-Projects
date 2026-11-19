/**
 * @file robot_config.hpp
 * @brief Starter hardware configuration and pin assignments for experimental robotics.
 *
 * Designed as a clean incubator template for rapid prototyping of Micromouse,
 * Balancing Robots, Drones, or Rovers on STM32F446RE (ARM Cortex-M4F @ 180 MHz).
 */

#pragma once

#include <cstdint>
#include "stm32/gpio/gpio.hpp"

namespace config {

// ==============================================================================
// System Clocks & Execution Rates
// ==============================================================================
inline constexpr std::uint32_t SYSTEM_CORE_CLOCK_HZ  = 180000000UL; // 180 MHz Cortex-M4F
inline constexpr std::uint32_t CONTROL_LOOP_HZ        = 1000UL;      // 1 kHz deterministic control loop
inline constexpr std::uint32_t CONTROL_PERIOD_MS     = 1000UL / CONTROL_LOOP_HZ;

// ==============================================================================
// Hardware Pin Mapping (Customizable per robot prototype: Micromouse, Drone, etc.)
// ==============================================================================

// Onboard Diagnostics / Status Heartbeat LED (Nucleo LD2 User LED: PA5)
inline constexpr gpio::Pin PIN_STATUS_LED     = gpio::PA5;

// User Button (Nucleo B1 User Button: PC13)
inline constexpr gpio::Pin PIN_USER_BUTTON    = gpio::PC13;

// Motor Driver 1 (e.g., Left Motor / Direction + PWM)
inline constexpr gpio::Pin PIN_MOTOR_L_IN1    = gpio::PB4;
inline constexpr gpio::Pin PIN_MOTOR_L_IN2    = gpio::PB5;
inline constexpr gpio::Pin PIN_MOTOR_L_PWM    = gpio::PA8; // TIM1_CH1

// Motor Driver 2 (e.g., Right Motor / Direction + PWM)
inline constexpr gpio::Pin PIN_MOTOR_R_IN1    = gpio::PB6;
inline constexpr gpio::Pin PIN_MOTOR_R_IN2    = gpio::PB7;
inline constexpr gpio::Pin PIN_MOTOR_R_PWM    = gpio::PA9; // TIM1_CH2

// Motor Standby / Driver Enable
inline constexpr gpio::Pin PIN_MOTOR_STANDBY  = gpio::PB0;

// Telemetry / Serial Communication (USART2: PA2 TX, PA3 RX)
inline constexpr gpio::Pin PIN_UART_TX        = gpio::PA2;
inline constexpr gpio::Pin PIN_UART_RX        = gpio::PA3;
inline constexpr std::uint32_t UART_BAUDRATE  = 115200UL;

} // namespace config
