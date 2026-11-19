/**
 * @file robot_app.hpp
 * @brief Robot application lifecycle and state machine template.
 */

#pragma once

#include <cstdint>
#include "robot_config.hpp"
#include "stm32/gpio/gpio.hpp"

namespace robot {

enum class State : std::uint8_t {
    INIT = 0,
    IDLE,
    CALIBRATING,
    RUNNING,
    ERROR
};

class RobotApp {
public:
    RobotApp();

    void init();
    void update(); // Called at deterministic 1 kHz tick

    [[nodiscard]] State getState() const noexcept { return state_; }
    [[nodiscard]] std::uint32_t getLoopCounter() const noexcept { return loop_counter_; }

private:
    void handleStateTransitions();
    void updateHeartbeat();

    State state_{State::INIT};
    std::uint32_t loop_counter_{0};
    std::uint32_t heartbeat_counter_{0};

    gpio::DigitalOutput status_led_{config::PIN_STATUS_LED};
    gpio::DigitalInput  user_button_{config::PIN_USER_BUTTON, gpio::Pull::UP};
};

} // namespace robot
