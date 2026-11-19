/**
 * @file battery_monitor.cpp
 * @brief Implementation of 3S LiPo battery monitor with IIR filter and alarm pattern.
 */

#include "battery_monitor.hpp"
#include "stm32/common/systick/systick.hpp"

namespace robot
{

BatteryMonitor::BatteryMonitor()
    : filteredVoltage_(config::VBAT_NOMINAL_VOLTS),
      status_(BatteryStatus::Normal),
      lastAlarmTimeMs_(0),
      initialized_(false)
{
}

bool BatteryMonitor::init(adc::AdcHandler<adc::Instance::adc1>& adc) noexcept
{
    // Enable ADC channel for battery voltage divider on PA4
    const bool ok = adc.enableChannel(config::PIN_VBAT_SENSE);
    if (ok)
    {
        // Initial prime sample
        const float initialRaw = adc.readVoltage(config::PIN_VBAT_SENSE, config::VBAT_ADC_VREF);
        filteredVoltage_ = initialRaw * config::VBAT_DIVIDER_RATIO;
        initialized_ = true;
    }
    return ok;
}

BatteryStatus BatteryMonitor::update(adc::AdcHandler<adc::Instance::adc1>& adc,
                                     components::actuators::buzzer::ActiveBuzzer* buzzer) noexcept
{
    if (!initialized_)
    {
        return BatteryStatus::Normal;
    }

    // Read current pin voltage and scale by divider ratio
    const float pinVoltage = adc.readVoltage(config::PIN_VBAT_SENSE, config::VBAT_ADC_VREF);
    const float rawVbat = pinVoltage * config::VBAT_DIVIDER_RATIO;

    // First-order low pass IIR filter (alpha = 0.90) to reject motor current draw dips
    constexpr float ALPHA = 0.90f;
    filteredVoltage_ = (ALPHA * filteredVoltage_) + ((1.0f - ALPHA) * rawVbat);

    const std::uint32_t now = systick::millis();

    if (filteredVoltage_ < config::VBAT_CRITICAL_VOLTS)
    {
        status_ = BatteryStatus::Critical;
        if (buzzer && (now - lastAlarmTimeMs_ >= 1000))
        {
            lastAlarmTimeMs_ = now;
            buzzer->playPattern(250, 100, 3); // 3 urgent long beeps
        }
    }
    else if (filteredVoltage_ < config::VBAT_WARNING_VOLTS)
    {
        status_ = BatteryStatus::Warning;
        if (buzzer && (now - lastAlarmTimeMs_ >= 3000))
        {
            lastAlarmTimeMs_ = now;
            buzzer->playPattern(80, 80, 2); // 2 warning beeps every 3 seconds
        }
    }
    else
    {
        status_ = BatteryStatus::Normal;
    }

    return status_;
}

float BatteryMonitor::getVoltage() const noexcept
{
    return filteredVoltage_;
}

BatteryStatus BatteryMonitor::getStatus() const noexcept
{
    return status_;
}

bool BatteryMonitor::isLowBattery() const noexcept
{
    return status_ != BatteryStatus::Normal;
}

bool BatteryMonitor::isCritical() const noexcept
{
    return status_ == BatteryStatus::Critical;
}

} // namespace robot
