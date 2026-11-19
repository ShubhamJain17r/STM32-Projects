/**
 * @file sensor_subsystem.hpp
 * @brief High-speed continuous 8-channel optical reflectance sensor pipeline.
 *
 * Implements:
 * - Continuous circular DMA acquisition via ADC1
 * - Active ambient light rejection via 2N7002 MOSFET (AutoDifferential mode)
 * - Seamless support for standard always-on clones
 * - Dynamic white-floor vs. black-line calibration
 * - Weighted centroid calculation with lost-line memory
 */

#pragma once

#include <cstdint>
#include <array>

#include "stm32/adc/adc.hpp"
#include "components/sensors/qtr/qtr_analog.hpp"
#include "robot_config.hpp"

namespace robot
{

class SensorSubsystem
{
public:
    explicit SensorSubsystem(bool useAutoDifferential = true);
    ~SensorSubsystem() = default;

    SensorSubsystem(const SensorSubsystem&) = delete;
    SensorSubsystem& operator=(const SensorSubsystem&) = delete;

    /**
     * @brief Initialize GPIOs, ADC1 sequence, and launch circular DMA acquisition.
     * @return true if initialization succeeded.
     */
    bool init() noexcept;

    /**
     * @brief Sample current surface and update calibration bounds from active DMA buffer.
     * Call repeatedly during the calibration sweep across track and floor.
     */
    void updateCalibration() noexcept;

    /**
     * @brief Reset dynamic calibration bounds.
     */
    void resetCalibration() noexcept;

    /**
     * @brief Check whether sensor array has been calibrated.
     */
    [[nodiscard]] bool isCalibrated() const noexcept;

    /**
     * @brief Read normalized line position and fill calibrated sensor output array.
     * Range: [0 .. 7000], where 3500 is dead center.
     *
     * @param[out] calibratedOut Array to receive normalized [0..1000] sensor reflections.
     * @return Estimated weighted centroid line position.
     */
    std::int32_t readLine(std::array<std::uint16_t, config::NUM_SENSORS>& calibratedOut) noexcept;

    /**
     * @brief Simplified readLine returning position without requiring user array.
     */
    std::int32_t readLine() noexcept;

    /**
     * @brief Get current steering error relative to track center (3500).
     * @return Error: 0 = centered, < 0 = left of line, > 0 = right of line.
     */
    [[nodiscard]] std::int32_t getSteeringError() noexcept;

    /**
     * @brief Check if the line is currently detected under the sensor array.
     */
    [[nodiscard]] bool isLineDetected() const noexcept;

    /**
     * @brief Get latest raw 12-bit ADC counts directly from the circular DMA buffer.
     */
    void getRawReadings(std::array<std::uint16_t, config::NUM_SENSORS>& rawOut) const noexcept;

    /**
     * @brief Access the underlying ADC handler (e.g. for reading battery voltage).
     */
    [[nodiscard]] adc::AdcHandler<adc::Instance::adc1>& getAdc() noexcept { return adc_; }

    [[nodiscard]] constexpr std::int32_t getCenterPosition() const noexcept
    {
        return config::SENSOR_CENTER_POSITION;
    }

private:
    adc::AdcHandler<adc::Instance::adc1>                     adc_;
    components::sensors::qtr::QtrAnalog<config::NUM_SENSORS> qtr_;

    // Hardware DMA target buffer (must be 32-bit aligned)
    alignas(4) volatile std::uint16_t                        dmaBuffer_[config::NUM_SENSORS];

    std::array<std::uint16_t, config::NUM_SENSORS>          lastCalibrated_;
    std::int32_t                                             lastPosition_;
};

} // namespace robot
