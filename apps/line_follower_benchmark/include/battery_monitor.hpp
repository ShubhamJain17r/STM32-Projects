/**
 * @file battery_monitor.hpp
 * @brief 3S LiPo battery voltage monitor with IIR filtering and multi-level alarms.
 *
 * Protects 3S LiPo cells from over-discharge:
 * - Voltage warning at < 9.9V (3.3V/cell)
 * - Critical emergency cutoff at < 9.3V (3.1V/cell)
 * - Low-pass IIR filtering to reject motor current inrush sag
 */

#pragma once

#include <cstdint>

#include "stm32/adc/adc.hpp"
#include "components/actuators/buzzer/buzzer.hpp"
#include "robot_config.hpp"

namespace robot
{

enum class BatteryStatus : std::uint8_t
{
    Normal = 0,
    Warning,    // Low battery (< 9.9V)
    Critical    // Critical cutoff (< 9.3V)
};

class BatteryMonitor
{
public:
    BatteryMonitor();
    ~BatteryMonitor() = default;

    /**
     * @brief Configure the ADC channel for voltage sensing.
     * @param adc Reference to initialized AdcHandler.
     */
    bool init(adc::AdcHandler<adc::Instance::adc1>& adc) noexcept;

    /**
     * @brief Periodic battery sampling and filtering.
     * Call at 10 Hz - 50 Hz.
     *
     * @param adc Reference to AdcHandler.
     * @param buzzer Optional reference to ActiveBuzzer for sounding low-voltage alarms.
     * @return Current BatteryStatus.
     */
    BatteryStatus update(adc::AdcHandler<adc::Instance::adc1>& adc,
                         components::actuators::buzzer::ActiveBuzzer* buzzer = nullptr) noexcept;

    [[nodiscard]] float getVoltage() const noexcept;
    [[nodiscard]] BatteryStatus getStatus() const noexcept;
    [[nodiscard]] bool isLowBattery() const noexcept;
    [[nodiscard]] bool isCritical() const noexcept;

private:
    float         filteredVoltage_;
    BatteryStatus status_;
    std::uint32_t lastAlarmTimeMs_;
    bool          initialized_;
};

} // namespace robot
