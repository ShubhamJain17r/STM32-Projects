/**
 * @file robot_config.hpp
 * @brief Centralized hardware pin definitions, kinematics, and tuning constants for High-Speed Line Follower.
 *
 * Configured for the Two-Board Modular PCB Architecture on STM32F446RE:
 * - Board 1: Mainboard (STM32F446RE, TB6612FNG bare IC, dual encoders, buck/LDO, buzzer, HC-05)
 * - Board 2: Front QTR Sensor Blade (8x Vishay QRE1113GR @ 7.5mm pitch + 2N7002 MOSFET emitter)
 */

#pragma once

#include <cstdint>
#include <array>

#include "stm32/gpio/gpio.hpp"
#include "stm32/adc/adc.hpp"
#include "stm32/timer/timer.hpp"
#include "stm32/uart/uart.hpp"

#include "components/common/component_base.hpp"

namespace robot::config
{

// =============================================================================
// 1. FRONT SENSOR BLADE CONFIGURATION (Vishay QRE1113GR Array + MOSFET)
// =============================================================================
constexpr std::size_t NUM_SENSORS = 8;

// Physical GPIO Pins connected to Sensor Blade Outputs (Analog Mode)
constexpr std::array<gpio::Pin, NUM_SENSORS> SENSOR_PINS = {
    gpio::PC0, // S1: Far Left       -> ADC1_IN10
    gpio::PC1, // S2: Left-Mid Outer -> ADC1_IN11
    gpio::PC2, // S3: Left-Mid Inner -> ADC1_IN12
    gpio::PC3, // S4: Center Left    -> ADC1_IN13
    gpio::PC4, // S5: Center Right   -> ADC1_IN14
    gpio::PC5, // S6: Right-Mid Inner-> ADC1_IN15
    gpio::PB0, // S7: Right-Mid Outer-> ADC1_IN8
    gpio::PB1  // S8: Far Right      -> ADC1_IN9
};

// Corresponding ADC1 Channels in scan sequence
constexpr std::array<adc::Channel, NUM_SENSORS> SENSOR_ADC_CHANNELS = {
    adc::Channel::IN10,
    adc::Channel::IN11,
    adc::Channel::IN12,
    adc::Channel::IN13,
    adc::Channel::IN14,
    adc::Channel::IN15,
    adc::Channel::IN8,
    adc::Channel::IN9
};

// 2N7002 N-Channel MOSFET Gate Pin for Active Ambient Light Rejection
constexpr gpio::Pin PIN_EMITTER_GATE = gpio::PB14;

// Sensor Geometry & Calculation Constants
constexpr float         SENSOR_PITCH_MM         = 7.5f;   // 7.5 mm sensor-to-sensor spacing
constexpr float         SENSOR_TOTAL_SPAN_MM    = 52.5f;  // (8 - 1) * 7.5 mm
constexpr float         SENSOR_GROUND_CLEAR_MM  = 3.0f;   // 3.0 mm +/- 0.5 mm ground clearance

constexpr std::int32_t  SENSOR_MIN_POSITION     = 0;
constexpr std::int32_t  SENSOR_CENTER_POSITION  = 3500;
constexpr std::int32_t  SENSOR_MAX_POSITION     = 7000;
constexpr std::uint32_t SENSOR_LOST_THRESHOLD   = 200;    // Sum of calibrated reflections below this = track lost
constexpr std::uint32_t SENSOR_CALIB_DURATION_MS = 3000;

// =============================================================================
// 2. MOTOR DRIVER ACTUATOR CONFIGURATION (TB6612FNG Bare IC on Mainboard)
// =============================================================================
// Timer Instance for 20 kHz Ultrasonic PWM
constexpr timer::Instance MOTOR_PWM_TIMER = timer::Instance::tim2;

// Motor A (Left Wheel)
constexpr gpio::Pin       PIN_PWMA = gpio::PA0; // TIM2_CH1 (AF1)
constexpr timer::Channel  CH_PWMA  = timer::Channel::CH1;
constexpr gpio::Pin       PIN_AIN1 = gpio::PC8; // Direction 1 (Atomic BSRR)
constexpr gpio::Pin       PIN_AIN2 = gpio::PC9; // Direction 2 (Atomic BSRR)

// Motor B (Right Wheel)
constexpr gpio::Pin       PIN_PWMB = gpio::PA1; // TIM2_CH2 (AF1)
constexpr timer::Channel  CH_PWMB  = timer::Channel::CH2;
constexpr gpio::Pin       PIN_BIN1 = gpio::PC11; // Direction 1 (Atomic BSRR)
constexpr gpio::Pin       PIN_BIN2 = gpio::PC12; // Direction 2 (Atomic BSRR)

// STBY is hardwired to 3.3V on the PCB trace (Zero GPIOs wasted!)
constexpr components::PinConnection MOTOR_STBY_CONNECTION = components::PinConnection::HardwareTiedHigh;

// PWM Frequency: 20 kHz (eliminates audible motor whine)
constexpr std::uint32_t   MOTOR_PWM_FREQUENCY_HZ = 20'000;

// Tuning & Stiction Compensation
constexpr float           MOTOR_DEADBAND = 0.05f; // Ignore tiny jitter near 0
constexpr float           MOTOR_MIN_DUTY = 0.12f; // Overcome N20 gearbox static friction

constexpr bool            MOTOR_A_INVERTED = false;
constexpr bool            MOTOR_B_INVERTED = false;

// =============================================================================
// 3. QUADRATURE ENCODERS (Silicon-Decoded via STM32 Timer Encoder Mode)
// =============================================================================
// Motor A (Left Wheel) Encoder: TIM3
constexpr timer::Instance ENC_L_TIMER = timer::Instance::tim3;
constexpr gpio::Pin       PIN_ENC_L_A = gpio::PA6; // TIM3_CH1 (AF2)
constexpr gpio::Pin       PIN_ENC_L_B = gpio::PA7; // TIM3_CH2 (AF2)

// Motor B (Right Wheel) Encoder: TIM4
constexpr timer::Instance ENC_R_TIMER = timer::Instance::tim4;
constexpr gpio::Pin       PIN_ENC_R_A = gpio::PB6; // TIM4_CH1 (AF2)
constexpr gpio::Pin       PIN_ENC_R_B = gpio::PB7; // TIM4_CH2 (AF2)

// =============================================================================
// 4. BATTERY VOLTAGE MONITOR (3S LiPo Pack Protection)
// =============================================================================
constexpr gpio::Pin       PIN_VBAT_SENSE = gpio::PA4; // ADC1_IN4 (Analog)
constexpr adc::Channel    CH_VBAT_SENSE  = adc::Channel::IN4;

// Voltage divider ratio: (R_top + R_bottom) / R_bottom
// Default 4.0: 12.6V input -> 3.15V at ADC pin
constexpr float           VBAT_DIVIDER_RATIO = 4.0f;
constexpr float           VBAT_ADC_VREF      = 3.3f;

// 3S LiPo Thresholds
constexpr float           VBAT_NOMINAL_VOLTS  = 11.1f;
constexpr float           VBAT_WARNING_VOLTS  = 9.9f;  // 3.3V/cell - trigger buzzer warning
constexpr float           VBAT_CRITICAL_VOLTS = 9.3f;  // 3.1V/cell - emergency cutoff

// =============================================================================
// 5. ACTIVE BUZZER (Audio Feedback & Low-Voltage Alarm)
// =============================================================================
constexpr gpio::Pin       PIN_BUZZER = gpio::PB8; // GPIO Output Push-Pull

// =============================================================================
// 6. WIRELESS TELEMETRY (HC-05 Bluetooth Module on Mainboard)
// =============================================================================
constexpr uart::Instance  TELEMETRY_UART = uart::Instance::usart1;
constexpr std::uint32_t   TELEMETRY_BAUDRATE = 9600;

// Alternate pins for USART1 to avoid collision with TIM4 encoders on PB6/PB7
constexpr gpio::Pin       PIN_BT_TX    = gpio::PA9;  // STM32 TX -> HC-05 RX (AF7)
constexpr gpio::Pin       PIN_BT_RX    = gpio::PA10; // STM32 RX <- HC-05 TX (AF7)
constexpr gpio::Pin       PIN_BT_STATE = gpio::PB2;  // Connection indicator

// =============================================================================
// 7. USER INTERFACE
// =============================================================================
constexpr gpio::Pin       PIN_STATUS_LED  = gpio::PA5;  // Onboard Green LED (LD2)
constexpr gpio::Pin       PIN_USER_BUTTON = gpio::PC13; // Onboard Blue Pushbutton (B1)

// =============================================================================
// 8. DETERMINISTIC CONTROL LOOP TIMEBASE
// =============================================================================
constexpr std::uint32_t   CONTROL_LOOP_FREQ_HZ = 1000; // 1.0 ms deterministic period
constexpr float           CONTROL_LOOP_DT_SEC  = 0.001f;

// =============================================================================
// 9. ROBOT KINEMATICS & CONTROL PARAMETERS
// =============================================================================
constexpr float           WHEEL_DIAMETER_METERS   = 0.042f; // 42 mm tires
constexpr float           WHEEL_CIRCUMFERENCE_M   = 0.13194689f;
constexpr std::uint32_t   ENCODER_COUNTS_PER_REV  = 1440;   // 12 CPR * 30:1 * 4x
constexpr float           METERS_PER_ENCODER_TICK = WHEEL_CIRCUMFERENCE_M / static_cast<float>(ENCODER_COUNTS_PER_REV);
constexpr float           MAX_LINEAR_VELOCITY_MPS = 1.32f;  // ~1.32 m/s @ 600 RPM

// Default Steering PID Parameters
constexpr float           DEFAULT_PID_KP          = 0.0006f;
constexpr float           DEFAULT_PID_KI          = 0.00001f;
constexpr float           DEFAULT_PID_KD          = 0.0003f;
constexpr float           DEFAULT_PID_FILTER_ALPHA= 0.65f;

// Dynamic Speed Scheduling Parameters
constexpr float           DEFAULT_BASE_SPEED      = 0.60f;  // 60% default straightaway throttle
constexpr float           MIN_CURVE_SPEED         = 0.30f;  // Minimum throttle on sharp turns
constexpr float           SPEED_CURVATURE_FACTOR  = 0.35f;  // Speed reduction gain per steering magnitude

} // namespace robot::config
