/**
 * @file main.cpp
 * @brief Robot Starter Template - Main Entry Point.
 *
 * Provides a clean, modular foundation for experimental robotics prototypes
 * (e.g., Micromouse, Balancing Robot, Drone, Rover).
 *
 * Implements:
 * - Deterministic 1 kHz (1.0 ms) SysTick-driven control loop
 * - Clean separation between hardware configuration, HAL, and robot logic
 * - State machine lifecycle (IDLE, RUNNING, ERROR)
 */

#include <cstdint>
#include "stm32/common/systick/systick.hpp"
#include "robot_config.hpp"
#include "robot_app.hpp"

int main()
{
    // Initialize SysTick monotonic 1.0 ms timebase (1 kHz)
    systick::init(config::CONTROL_LOOP_HZ);

    // Instantiate Robot Application
    robot::RobotApp app;
    app.init();

    std::uint32_t last_loop_time = systick::millis();

    while (true)
    {
        const std::uint32_t current_time = systick::millis();

        // Deterministic 1 kHz Control Loop Execution (every 1 ms)
        if ((current_time - last_loop_time) >= config::CONTROL_PERIOD_MS)
        {
            last_loop_time = current_time;
            app.update();
        }
    }

    return 0;
}
