/**
 * @file telemetry_manager.cpp
 * @brief Implementation of HC-05 packet parser and telemetry formatter.
 */

#include "telemetry_manager.hpp"

#include <cstring>
#include <cstdio>
#include <cstdlib>

namespace robot
{

TelemetryManager::TelemetryManager()
    : uart_(config::PIN_BT_TX, config::PIN_BT_RX, config::TELEMETRY_BAUDRATE),
      bt_(uart_, gpio::Pin{nullptr, 0xFF}, config::PIN_BT_STATE),
      packetBuffer_{0}
{
}

bool TelemetryManager::init() noexcept
{
    bt_.init();
    sendLine("==============================================");
    sendLine("STM32 Closed-Loop Line Follower Robot Online");
    sendLine("Commands: <START>, <STOP>, <CALIB>, <KP:val>, <KD:val>, <SPEED:val>");
    sendLine("==============================================");
    return true;
}

Command TelemetryManager::pollCommand() noexcept
{
    Command cmd{CommandType::None, 0.0f};

    // Read framed packet <COMMAND:DATA> with a non-blocking 2 ms timeout
    const std::size_t len = bt_.readPacket('<', '>', packetBuffer_, sizeof(packetBuffer_), 2);
    if (len == 0)
    {
        return cmd;
    }

    if (std::strcmp(packetBuffer_, "START") == 0)
    {
        cmd.type = CommandType::Start;
        sendLine("ACK: Robot STARTED");
    }
    else if (std::strcmp(packetBuffer_, "STOP") == 0)
    {
        cmd.type = CommandType::Stop;
        sendLine("ACK: Robot STOPPED");
    }
    else if (std::strcmp(packetBuffer_, "CALIB") == 0)
    {
        cmd.type = CommandType::Calibrate;
        sendLine("ACK: Calibration Triggered");
    }
    else if (std::strncmp(packetBuffer_, "KP:", 3) == 0)
    {
        cmd.type  = CommandType::SetKp;
        cmd.value = std::strtof(&packetBuffer_[3], nullptr);
        char ack[40];
        std::snprintf(ack, sizeof(ack), "ACK: Kp = %.6f", static_cast<double>(cmd.value));
        sendLine(ack);
    }
    else if (std::strncmp(packetBuffer_, "KI:", 3) == 0)
    {
        cmd.type  = CommandType::SetKi;
        cmd.value = std::strtof(&packetBuffer_[3], nullptr);
        char ack[40];
        std::snprintf(ack, sizeof(ack), "ACK: Ki = %.6f", static_cast<double>(cmd.value));
        sendLine(ack);
    }
    else if (std::strncmp(packetBuffer_, "KD:", 3) == 0)
    {
        cmd.type  = CommandType::SetKd;
        cmd.value = std::strtof(&packetBuffer_[3], nullptr);
        char ack[40];
        std::snprintf(ack, sizeof(ack), "ACK: Kd = %.6f", static_cast<double>(cmd.value));
        sendLine(ack);
    }
    else if (std::strncmp(packetBuffer_, "SPEED:", 6) == 0)
    {
        cmd.type  = CommandType::SetSpeed;
        cmd.value = std::strtof(&packetBuffer_[6], nullptr);
        char ack[40];
        std::snprintf(ack, sizeof(ack), "ACK: BaseSpeed = %.2f", static_cast<double>(cmd.value));
        sendLine(ack);
    }

    return cmd;
}

void TelemetryManager::sendLine(const char* message) noexcept
{
    bt_.sendLine(message);
}

void TelemetryManager::sendTelemetry(const char* stateName,
                                     float leftRpm,
                                     float rightRpm,
                                     float distanceMeters,
                                     float batteryVoltage,
                                     std::int32_t steeringError,
                                     float commandedSteering) noexcept
{
    char telem[110];
    std::snprintf(telem, sizeof(telem),
                  "[TELEM] %s L:%.0f R:%.0f D:%.2fm V:%.2fV Err:%ld Str:%.2f",
                  stateName,
                  static_cast<double>(leftRpm),
                  static_cast<double>(rightRpm),
                  static_cast<double>(distanceMeters),
                  static_cast<double>(batteryVoltage),
                  static_cast<long>(steeringError),
                  static_cast<double>(commandedSteering));
    bt_.sendLine(telem);
}

bool TelemetryManager::isConnected() const noexcept
{
    return bt_.isConnected();
}

} // namespace robot
