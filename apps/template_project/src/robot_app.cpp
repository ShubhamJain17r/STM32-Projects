/**
 * @file robot_app.cpp
 * @brief Robot application lifecycle and state machine template implementation.
 */

#include "robot_app.hpp"

namespace robot {

RobotApp::RobotApp() = default;

void RobotApp::init()
{
    status_led_.low();
    state_ = State::IDLE;
    loop_counter_ = 0;
    heartbeat_counter_ = 0;
}

void RobotApp::update()
{
    loop_counter_++;
    heartbeat_counter_++;

    handleStateTransitions();
    updateHeartbeat();
}

void RobotApp::handleStateTransitions()
{
    // Check user button (active LOW when pulled up)
    const bool button_pressed = user_button_.isLow();

    switch (state_) {
        case State::INIT:
            state_ = State::IDLE;
            break;

        case State::IDLE:
            if (button_pressed) {
                state_ = State::RUNNING;
            }
            break;

        case State::RUNNING:
            if (button_pressed && (heartbeat_counter_ > 500)) {
                // Hold/press again to return to IDLE
                state_ = State::IDLE;
                heartbeat_counter_ = 0;
            }
            break;

        case State::CALIBRATING:
        case State::ERROR:
        default:
            break;
    }
}

void RobotApp::updateHeartbeat()
{
    // Flash rate varies by state:
    // IDLE: 1 Hz heartbeat (toggle every 500 ms)
    // RUNNING: 5 Hz heartbeat (toggle every 100 ms)
    const std::uint32_t toggle_period_ms = (state_ == State::RUNNING) ? 100 : 500;

    if (heartbeat_counter_ % toggle_period_ms == 0) {
        status_led_.toggle();
    }
}

} // namespace robot
