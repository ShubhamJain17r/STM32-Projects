/**
 * @file telemetry_manager.hpp
 * @brief Non-blocking HC-05 Bluetooth telemetry stream and command packet parser.
 *
 * Supports wireless live tuning:
 * - Commands: <START>, <STOP>, <CALIB>, <KP:val>, <KI:val>, <KD:val>, <SPEED:val>
 * - Telemetry: State, Speed, Wheel RPM, Traveled Distance, Battery Voltage, Steering Error
 */

#pragma once

#include <cstdint>

#include "stm32/uart/uart.hpp"
#include "components/wireless/hc05/hc05.hpp"
#include "components/common/pid_controller.hpp"
#include "robot_config.hpp"

namespace robot
{

enum class CommandType : std::uint8_t
{
    None = 0,
    Start,
    Stop,
    Calibrate,
    SetKp,
    SetKi,
    SetKd,
    SetSpeed
};

struct Command
{
    CommandType type  = CommandType::None;
    float       value = 0.0f;
};

class TelemetryManager
{
public:
    TelemetryManager();
    ~TelemetryManager() = default;

    TelemetryManager(const TelemetryManager&) = delete;
    TelemetryManager& operator=(const TelemetryManager&) = delete;

    /**
     * @brief Initialize USART1 at 9600 baud and bind HC-05 driver.
     * @return true if initialization succeeded.
     */
    bool init() noexcept;

    /**
     * @brief Poll incoming Bluetooth buffer for framed packets: <CMD:VALUE>.
     * Non-blocking (returns immediately if no complete packet is waiting).
     *
     * @return Parsed Command struct.
     */
    Command pollCommand() noexcept;

    /**
     * @brief Send a text line over Bluetooth (appends \r\n).
     */
    void sendLine(const char* message) noexcept;

    /**
     * @brief Format and send periodic telemetry packet.
     */
    void sendTelemetry(const char* stateName,
                       float leftRpm,
                       float rightRpm,
                       float distanceMeters,
                       float batteryVoltage,
                       std::int32_t steeringError,
                       float commandedSteering) noexcept;

    [[nodiscard]] bool isConnected() const noexcept;

private:
    uart::UartHandler<config::TELEMETRY_UART, 128, 128>            uart_;
    components::wireless::hc05::Hc05<config::TELEMETRY_UART, 128, 128> bt_;

    char packetBuffer_[48];
};

} // namespace robot
