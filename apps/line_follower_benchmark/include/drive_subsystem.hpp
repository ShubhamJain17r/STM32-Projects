/**
 * @file drive_subsystem.hpp
 * @brief Integrated dual DC motor actuator and hardware quadrature encoder pipeline.
 *
 * Implements:
 * - 20 kHz ultrasonic PWM on TIM2 (eliminates audible motor whine)
 * - TB6612FNG bare-IC mode with STBY hardwired to 3.3V on PCB (0 GPIOs wasted)
 * - Dual hardware timer quadrature encoders (TIM3 for left, TIM4 for right) with 0% CPU decoding
 * - Differential steering mixer with stiction and deadband compensation
 * - Dynamic short-braking for safety recovery
 * - Real-time wheel velocity (RPM, m/s) and distance odometry
 */

#pragma once

#include <cstdint>

#include "stm32/timer/timer.hpp"
#include "components/actuators/tb6612fng/tb6612fng.hpp"
#include "components/actuators/encoder/encoder.hpp"
#include "robot_config.hpp"

namespace robot
{

class DriveSubsystem
{
public:
    DriveSubsystem();
    ~DriveSubsystem() = default;

    DriveSubsystem(const DriveSubsystem&) = delete;
    DriveSubsystem& operator=(const DriveSubsystem&) = delete;

    /**
     * @brief Initialize 20 kHz PWM timer, direction GPIOs, and both hardware encoders.
     * @return true if initialization succeeded.
     */
    bool init() noexcept;

    /**
     * @brief Update encoder tick accumulation, velocity, and distance calculations.
     * Call at a periodic rate (e.g. 500 Hz or 1 kHz).
     *
     * @param nowMs Monotonic millisecond timestamp from systick::millis().
     */
    void updateEncoders(std::uint32_t nowMs) noexcept;

    /**
     * @brief Set normalized motor speeds in range [-1.0f, +1.0f].
     *
     * @param leftSpeed Left motor speed (-1.0 = full reverse, +1.0 = full forward)
     * @param rightSpeed Right motor speed (-1.0 = full reverse, +1.0 = full forward)
     */
    void setSpeeds(float leftSpeed, float rightSpeed) noexcept;

    /**
     * @brief Coordinated differential drive steering mixer.
     *
     * @param throttle Forward/reverse throttle in [-1.0f, +1.0f]
     * @param steering Steering bias in [-1.0f, +1.0f] (positive turns right, negative turns left)
     */
    void drive(float throttle, float steering) noexcept;

    /**
     * @brief Actively short-brake both motors (shorts H-bridge outputs to ground).
     */
    void brake() noexcept;

    /**
     * @brief Freewheel coast both motors (High-Z floating outputs).
     */
    void coast() noexcept;

    // Odometry & Kinematics APIs
    [[nodiscard]] float getLeftRpm() const noexcept;
    [[nodiscard]] float getRightRpm() const noexcept;
    [[nodiscard]] float getAverageRpm() const noexcept;

    [[nodiscard]] float getLeftSpeedMps() const noexcept;
    [[nodiscard]] float getRightSpeedMps() const noexcept;
    [[nodiscard]] float getAverageSpeedMps() const noexcept;

    [[nodiscard]] float getDistanceMeters() const noexcept;
    void resetDistance() noexcept;

    [[nodiscard]] float getLeftCommandedSpeed() const noexcept;
    [[nodiscard]] float getRightCommandedSpeed() const noexcept;

private:
    timer::PwmHandler<config::MOTOR_PWM_TIMER>                           pwm_;
    components::actuators::tb6612fng::Tb6612Fng<config::MOTOR_PWM_TIMER> motors_;

    components::actuators::encoder::QuadratureEncoder<config::ENC_L_TIMER> leftEncoder_;
    components::actuators::encoder::QuadratureEncoder<config::ENC_R_TIMER> rightEncoder_;
};

} // namespace robot
