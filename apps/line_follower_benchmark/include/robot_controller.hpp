/**
 * @file robot_controller.hpp
 * @brief Top-level Finite State Machine and Closed-Loop Control Orchestrator.
 *
 * Coordinates:
 * - SensorSubsystem (DMA optical acquisition & AutoDifferential ambient rejection)
 * - DriveSubsystem (20 kHz PWM & dual TIM3/TIM4 quadrature encoders)
 * - BatteryMonitor (3S LiPo voltage & low-voltage protection)
 * - TelemetryManager (HC-05 SPP streaming & PID tuning packet parser)
 * - ActiveBuzzer (Non-blocking auditory feedback)
 * - PidController (1 kHz discrete anti-windup steering controller)
 */

#pragma once

#include <cstdint>

#include "robot_config.hpp"
#include "sensor_subsystem.hpp"
#include "drive_subsystem.hpp"
#include "battery_monitor.hpp"
#include "telemetry_manager.hpp"

#include "components/actuators/buzzer/buzzer.hpp"
#include "components/common/pid_controller.hpp"

namespace robot
{

enum class RobotState : std::uint8_t
{
    Init = 0,
    Calibration,
    Ready,
    Tracking,
    LostRecovery,
    EmergencyStop
};

class RobotController
{
public:
    RobotController();
    ~RobotController() = default;

    /**
     * @brief Initialize all hardware peripherals, sensors, timers, and telemetry.
     * @return true if all subsystems initialized cleanly.
     */
    bool init() noexcept;

    /**
     * @brief High-frequency closed loop step (called at 1 kHz / 1.0 ms).
     * @param nowMs Monotonic millisecond timestamp from systick::millis().
     * @param dtSec Elapsed time in seconds since last control step.
     */
    void stepControl(std::uint32_t nowMs, float dtSec) noexcept;

    /**
     * @brief Background tasks step (telemetry, battery check, button poll, buzzer updates).
     * Called in main loop.
     * @param nowMs Monotonic millisecond timestamp.
     */
    void stepBackground(std::uint32_t nowMs) noexcept;

    [[nodiscard]] RobotState getState() const noexcept;
    const char* getStateName() const noexcept;

    void start() noexcept;
    void stop() noexcept;
    void startCalibration() noexcept;

private:
    SensorSubsystem                               sensors_;
    DriveSubsystem                                drive_;
    BatteryMonitor                                battery_;
    TelemetryManager                              telemetry_;
    components::actuators::buzzer::ActiveBuzzer   buzzer_;
    components::control::PidController            steeringPid_;

    RobotState    state_;
    float         baseSpeed_;
    std::uint32_t stateEnterTimeMs_;
    std::uint32_t lostStartTimeMs_;
    std::uint32_t lastTelemTimeMs_;
    std::uint32_t lastBatteryCheckMs_;

    std::int32_t  lastError_;
    float         lastSteeringOutput_;

    void transitionTo(RobotState newState, std::uint32_t nowMs) noexcept;
    void handleCommand(const Command& cmd, std::uint32_t nowMs) noexcept;
};

} // namespace robot
