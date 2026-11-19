/**
 * @file robot_controller.cpp
 * @brief Implementation of top-level robot finite state machine and control loops.
 */

#include "robot_controller.hpp"
#include "components/common/math_utils.hpp"

namespace robot
{

RobotController::RobotController()
    : sensors_(true), // AutoDifferential active ambient rejection
      drive_(),
      battery_(),
      telemetry_(),
      buzzer_(config::PIN_BUZZER),
      steeringPid_(components::control::PidConfig{
          config::DEFAULT_PID_KP,
          config::DEFAULT_PID_KI,
          config::DEFAULT_PID_KD,
          0.0f, // kf
          -1.0f, 1.0f, // outMin, outMax
          -0.4f, 0.4f, // intMin, intMax
          config::DEFAULT_PID_FILTER_ALPHA
      }),
      state_(RobotState::Init),
      baseSpeed_(config::DEFAULT_BASE_SPEED),
      stateEnterTimeMs_(0),
      lostStartTimeMs_(0),
      lastTelemTimeMs_(0),
      lastBatteryCheckMs_(0),
      lastError_(0),
      lastSteeringOutput_(0.0f)
{
}

bool RobotController::init() noexcept
{
    // 1. Initialize active buzzer
    buzzer_.init();

    // 2. Initialize 8-channel optical sensor DMA pipeline
    if (!sensors_.init())
    {
        return false;
    }

    // 3. Initialize dual 20 kHz PWM motors and hardware quadrature encoders
    if (!drive_.init())
    {
        return false;
    }

    // 4. Initialize 3S LiPo battery monitor on ADC1
    if (!battery_.init(sensors_.getAdc()))
    {
        return false;
    }

    // 5. Initialize HC-05 Bluetooth telemetry and command interface
    if (!telemetry_.init())
    {
        return false;
    }

    // 6. Audio notification: 2 short beeps indicating successful hardware boot
    buzzer_.playPattern(80, 80, 2);

    // Enter calibration phase immediately on startup
    startCalibration();

    return true;
}

void RobotController::transitionTo(RobotState newState, std::uint32_t nowMs) noexcept
{
    state_ = newState;
    stateEnterTimeMs_ = nowMs;

    if (newState == RobotState::EmergencyStop || newState == RobotState::Ready)
    {
        drive_.brake();
    }
}

void RobotController::startCalibration() noexcept
{
    sensors_.resetCalibration();
    buzzer_.playPattern(100, 50, 1);
    transitionTo(RobotState::Calibration, 0);
}

void RobotController::start() noexcept
{
    if (state_ == RobotState::Ready || state_ == RobotState::EmergencyStop)
    {
        steeringPid_.reset();
        drive_.resetDistance();
        buzzer_.playPattern(150, 50, 1);
        transitionTo(RobotState::Tracking, 0);
    }
}

void RobotController::stop() noexcept
{
    drive_.brake();
    buzzer_.playPattern(200, 50, 1);
    transitionTo(RobotState::EmergencyStop, 0);
}

void RobotController::stepControl(std::uint32_t nowMs, float dtSec) noexcept
{
    // 1. Update hardware timer quadrature encoder odometry (0% CPU decoding)
    drive_.updateEncoders(nowMs);

    // 2. State-dependent closed-loop behavior
    switch (state_)
    {
        case RobotState::Tracking:
        {
            // Sample weighted centroid directly from circular DMA buffer
            const std::int32_t error = sensors_.getSteeringError();
            const bool lineDetected  = sensors_.isLineDetected();

            if (lineDetected)
            {
                lastError_ = error;

                // Compute steering correction via discrete PID controller
                const float steering = steeringPid_.updateError(static_cast<float>(error), dtSec);
                lastSteeringOutput_ = steering;

                // Dynamic speed scheduling: decelerate on sharp curves
                const float speedReduction = config::SPEED_CURVATURE_FACTOR * components::math::abs(steering);
                float targetSpeed = baseSpeed_ - speedReduction;
                if (targetSpeed < config::MIN_CURVE_SPEED)
                {
                    targetSpeed = config::MIN_CURVE_SPEED;
                }

                drive_.drive(targetSpeed, steering);
            }
            else
            {
                // Line lost on sharp bend or chicane -> enter recovery mode
                lostStartTimeMs_ = nowMs;
                transitionTo(RobotState::LostRecovery, nowMs);
            }
            break;
        }

        case RobotState::LostRecovery:
        {
            // If track remains lost for more than 500 ms, trigger emergency brake
            if ((nowMs - lostStartTimeMs_) > 500)
            {
                drive_.brake();
                buzzer_.playPattern(300, 100, 2);
                transitionTo(RobotState::EmergencyStop, nowMs);
                break;
            }

            // Check if line is re-acquired
            const std::int32_t error = sensors_.getSteeringError();
            if (sensors_.isLineDetected())
            {
                transitionTo(RobotState::Tracking, nowMs);
            }
            else
            {
                // Lost-line memory: steer aggressively toward the side where line was last detected
                const float recoverySteer = (error < 0) ? -0.85f : 0.85f;
                constexpr float SEARCH_THROTTLE = 0.28f;
                drive_.drive(SEARCH_THROTTLE, recoverySteer);
            }
            break;
        }

        case RobotState::Calibration:
        {
            // Continuously update white-minimum / black-maximum thresholds
            sensors_.updateCalibration();

            if (stateEnterTimeMs_ == 0)
            {
                stateEnterTimeMs_ = nowMs;
            }

            if ((nowMs - stateEnterTimeMs_) >= config::SENSOR_CALIB_DURATION_MS)
            {
                // Calibration complete chime
                buzzer_.playPattern(150, 50, 2);
                transitionTo(RobotState::Ready, nowMs);
            }
            break;
        }

        case RobotState::Ready:
        case RobotState::EmergencyStop:
        case RobotState::Init:
        default:
        {
            drive_.brake();
            break;
        }
    }
}

void RobotController::stepBackground(std::uint32_t nowMs) noexcept
{
    // 1. Update non-blocking buzzer state machine
    buzzer_.update(nowMs);

    // 2. Poll incoming Bluetooth wireless commands (<START>, <STOP>, <KP:...>, etc.)
    const Command cmd = telemetry_.pollCommand();
    if (cmd.type != CommandType::None)
    {
        handleCommand(cmd, nowMs);
    }

    // 3. Periodic 3S LiPo battery voltage check (10 Hz = 100 ms)
    if ((nowMs - lastBatteryCheckMs_) >= 100)
    {
        lastBatteryCheckMs_ = nowMs;
        battery_.update(sensors_.getAdc(), &buzzer_);

        if (battery_.isCritical() && state_ != RobotState::EmergencyStop)
        {
            drive_.brake();
            transitionTo(RobotState::EmergencyStop, nowMs);
            telemetry_.sendLine("ALARM: CRITICAL LOW BATTERY CUTOFF (< 9.3V)");
        }
    }

    // 4. Periodic Telemetry Streaming (5 Hz = 200 ms)
    if ((nowMs - lastTelemTimeMs_) >= 200)
    {
        lastTelemTimeMs_ = nowMs;
        telemetry_.sendTelemetry(getStateName(),
                                 drive_.getLeftRpm(),
                                 drive_.getRightRpm(),
                                 drive_.getDistanceMeters(),
                                 battery_.getVoltage(),
                                 lastError_,
                                 lastSteeringOutput_);
    }
}

void RobotController::handleCommand(const Command& cmd, [[maybe_unused]] std::uint32_t nowMs) noexcept
{
    switch (cmd.type)
    {
        case CommandType::Start:
            start();
            break;

        case CommandType::Stop:
            stop();
            break;

        case CommandType::Calibrate:
            startCalibration();
            break;

        case CommandType::SetKp:
            steeringPid_.setGains(cmd.value, steeringPid_.getKi(), steeringPid_.getKd(), steeringPid_.getKf());
            break;

        case CommandType::SetKi:
            steeringPid_.setGains(steeringPid_.getKp(), cmd.value, steeringPid_.getKd(), steeringPid_.getKf());
            break;

        case CommandType::SetKd:
            steeringPid_.setGains(steeringPid_.getKp(), steeringPid_.getKi(), cmd.value, steeringPid_.getKf());
            break;

        case CommandType::SetSpeed:
            baseSpeed_ = components::math::clamp(cmd.value, 0.1f, 1.0f);
            break;

        case CommandType::None:
        default:
            break;
    }
}

RobotState RobotController::getState() const noexcept
{
    return state_;
}

const char* RobotController::getStateName() const noexcept
{
    switch (state_)
    {
        case RobotState::Init:          return "INIT";
        case RobotState::Calibration:   return "CALIB";
        case RobotState::Ready:         return "READY";
        case RobotState::Tracking:      return "TRACK";
        case RobotState::LostRecovery:  return "LOST";
        case RobotState::EmergencyStop: return "STOP";
        default:                        return "UNKNOWN";
    }
}

} // namespace robot
