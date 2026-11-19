/**
 * @file main.cpp
 * @brief High-Speed Closed-Loop Line Following Robot - Main Application Entry Point.
 *
 * Implements:
 * - Deterministic 1 kHz (1.0 ms) closed-loop control loop
 * - Dynamic curvature speed scheduling
 * - Silicon-decoded quadrature encoder odometry (TIM3/TIM4)
 * - Continuous circular DMA optical sensor acquisition & ambient light rejection
 * - Wireless HC-05 live parameter tuning (<KP:...>, <KD:...>, <SPEED:...>)
 * - 3S LiPo battery safety monitoring with audio buzzer alarms
 */

#include <cstdint>

#include "stm32/common/systick/systick.hpp"
#include "stm32/gpio/gpio.hpp"

#include "robot_config.hpp"
#include "robot_controller.hpp"

using namespace robot;

int main()
{
    // -------------------------------------------------------------------------
    // 1. Core System Timebase & UI Initialization
    // -------------------------------------------------------------------------
    // Initialize SysTick monotonic 1.0 ms timebase
    systick::init(1000);

    // Initialize Onboard User LED (PA5) and User Pushbutton (PC13)
    gpio::DigitalOutput statusLed{config::PIN_STATUS_LED};
    gpio::DigitalInput  userButton{config::PIN_USER_BUTTON, gpio::Pull::UP};

    statusLed.high(); // Solid ON during boot

    // -------------------------------------------------------------------------
    // 2. Instantiate and Initialize Robot Controller
    // -------------------------------------------------------------------------
    RobotController robot;
    if (!robot.init())
    {
        // Fault state: rapid blinking indicates hardware initialization failure
        while (true)
        {
            statusLed.toggle();
            systick::delayMs(50);
        }
    }

    // -------------------------------------------------------------------------
    // 3. Main Deterministic Control & Scheduling Loop
    // -------------------------------------------------------------------------
    std::uint32_t lastControlTime = systick::millis();
    std::uint32_t lastLedBlinkTime = systick::millis();
    bool lastButtonStateHigh = true;

    while (true)
    {
        const std::uint32_t now = systick::millis();

        // A. 1 kHz Deterministic Control Loop Step (every 1.0 ms)
        if ((now - lastControlTime) >= 1)
        {
            const float dtSec = static_cast<float>(now - lastControlTime) / 1000.0f;
            lastControlTime = now;

            robot.stepControl(now, dtSec);
        }

        // B. Non-Blocking Background Tasks (telemetry, battery, buzzer, wireless cmds)
        robot.stepBackground(now);

        // C. User Pushbutton Debounce & Action
        const bool buttonPressed = (userButton.read() == gpio::PinState::LOW);
        if (buttonPressed && lastButtonStateHigh)
        {
            // Button pressed (falling edge): toggle start / stop
            if (robot.getState() == RobotState::Tracking)
            {
                robot.stop();
            }
            else if (robot.getState() == RobotState::Ready || robot.getState() == RobotState::EmergencyStop)
            {
                robot.start();
            }
        }
        lastButtonStateHigh = !buttonPressed;

        // D. Visual Status LED Indication
        switch (robot.getState())
        {
            case RobotState::Tracking:
                statusLed.high(); // Solid ON when actively tracking
                break;

            case RobotState::Calibration:
                // Rapid 10 Hz blink during sensor blade calibration sweep
                if ((now - lastLedBlinkTime) >= 50)
                {
                    lastLedBlinkTime = now;
                    statusLed.toggle();
                }
                break;

            case RobotState::Ready:
                // Gentle 2 Hz heartbeat pulse when armed and ready
                if ((now - lastLedBlinkTime) >= 250)
                {
                    lastLedBlinkTime = now;
                    statusLed.toggle();
                }
                break;

            case RobotState::EmergencyStop:
            case RobotState::LostRecovery:
            default:
                statusLed.low();
                break;
        }
    }

    return 0;
}
