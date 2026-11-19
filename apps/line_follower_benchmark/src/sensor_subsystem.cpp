/**
 * @file sensor_subsystem.cpp
 * @brief Implementation of continuous circular DMA sensor pipeline with ambient rejection.
 */

#include "sensor_subsystem.hpp"

namespace robot
{

SensorSubsystem::SensorSubsystem(bool useAutoDifferential)
    : adc_(adc::Resolution::Bits12, adc::SamplingTime::Cycles84),
      qtr_(config::SENSOR_PINS,
           config::PIN_EMITTER_GATE,
           components::sensors::qtr::LineColor::DarkOnLight,
           true, // Active-High gate for N-MOSFET (2N7002)
           useAutoDifferential ? components::sensors::qtr::EmitterPolicy::AutoDifferential
                               : components::sensors::qtr::EmitterPolicy::AlwaysOn),
      dmaBuffer_{0},
      lastCalibrated_{0},
      lastPosition_(config::SENSOR_CENTER_POSITION)
{
}

bool SensorSubsystem::init() noexcept
{
    // 1. Initialize sensor GPIO pins to ANALOG mode and emitter pin to OUTPUT
    if (!qtr_.init())
    {
        return false;
    }

    // 2. Configure ADC1 sequence for all 8 channels
    adc_.configureSequence(config::SENSOR_ADC_CHANNELS.data(), config::NUM_SENSORS);

    // 3. Set lost-line reflection sum threshold
    qtr_.setLostLineThreshold(config::SENSOR_LOST_THRESHOLD);

    // 4. Launch continuous circular DMA conversion into dmaBuffer_
    adc_.startDma(const_cast<std::uint16_t*>(dmaBuffer_),
                  config::NUM_SENSORS,
                  nullptr,
                  true);

    return true;
}

void SensorSubsystem::updateCalibration() noexcept
{
    std::array<std::uint16_t, config::NUM_SENSORS> raw;
    getRawReadings(raw);
    qtr_.updateCalibration(raw);
}

void SensorSubsystem::resetCalibration() noexcept
{
    qtr_.resetCalibration();
}

bool SensorSubsystem::isCalibrated() const noexcept
{
    return qtr_.isCalibrated();
}

std::int32_t SensorSubsystem::readLine(std::array<std::uint16_t, config::NUM_SENSORS>& calibratedOut) noexcept
{
    // Read directly from DMA buffer without blocking CPU
    lastPosition_ = qtr_.readLineDma(const_cast<const std::uint16_t*>(dmaBuffer_), calibratedOut);
    lastCalibrated_ = calibratedOut;
    return lastPosition_;
}

std::int32_t SensorSubsystem::readLine() noexcept
{
    return readLine(lastCalibrated_);
}

std::int32_t SensorSubsystem::getSteeringError() noexcept
{
    return readLine() - config::SENSOR_CENTER_POSITION;
}

bool SensorSubsystem::isLineDetected() const noexcept
{
    return qtr_.isLineDetected();
}

void SensorSubsystem::getRawReadings(std::array<std::uint16_t, config::NUM_SENSORS>& rawOut) const noexcept
{
    for (std::size_t i = 0; i < config::NUM_SENSORS; ++i)
    {
        rawOut[i] = dmaBuffer_[i];
    }
}

} // namespace robot
