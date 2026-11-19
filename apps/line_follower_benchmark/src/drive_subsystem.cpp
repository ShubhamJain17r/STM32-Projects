/**
 * @file drive_subsystem.cpp
 * @brief Implementation of integrated motor drive and hardware encoder subsystem.
 */

#include "drive_subsystem.hpp"

namespace robot
{

namespace
{
components::actuators::tb6612fng::ChannelConfig makeMotorConfig(bool inverted) noexcept
{
    components::actuators::tb6612fng::ChannelConfig cfg;
    cfg.inverted      = inverted;
    cfg.deadband      = config::MOTOR_DEADBAND;
    cfg.minDuty       = config::MOTOR_MIN_DUTY;
    cfg.zeroSpeedMode = components::actuators::tb6612fng::ZeroSpeedMode::Brake;
    return cfg;
}

components::actuators::encoder::EncoderConfig makeEncoderConfig(bool inverted) noexcept
{
    components::actuators::encoder::EncoderConfig cfg;
    cfg.countsPerRevolution = config::ENCODER_COUNTS_PER_REV;
    cfg.wheelDiameterMeters = config::WHEEL_DIAMETER_METERS;
    cfg.inverted            = inverted;
    return cfg;
}
} // anonymous namespace

DriveSubsystem::DriveSubsystem()
    : pwm_(config::MOTOR_PWM_FREQUENCY_HZ),
      motors_(config::PIN_AIN1, config::PIN_AIN2, pwm_, config::CH_PWMA,
              config::PIN_BIN1, config::PIN_BIN2, pwm_, config::CH_PWMB,
              config::MOTOR_STBY_CONNECTION,
              makeMotorConfig(config::MOTOR_A_INVERTED),
              makeMotorConfig(config::MOTOR_B_INVERTED)),
      leftEncoder_(config::PIN_ENC_L_A, config::PIN_ENC_L_B, makeEncoderConfig(false)),
      rightEncoder_(config::PIN_ENC_R_A, config::PIN_ENC_R_B, makeEncoderConfig(false))
{
}

bool DriveSubsystem::init() noexcept
{
    // 1. Start TIM2 counter for 20 kHz ultrasonic PWM
    pwm_.start();

    // 2. Initialize TB6612 direction pins and PWM channels
    const bool motorOk = motors_.init(config::PIN_PWMA, config::PIN_PWMB);

    // 3. Ensure motors start in short-braked state
    motors_.brake();

    // 4. Initialize hardware timer quadrature encoders (TIM3 and TIM4)
    const bool encLOk = leftEncoder_.init();
    const bool encROk = rightEncoder_.init();

    return motorOk && encLOk && encROk;
}

void DriveSubsystem::updateEncoders(std::uint32_t nowMs) noexcept
{
    leftEncoder_.update(nowMs);
    rightEncoder_.update(nowMs);
}

void DriveSubsystem::setSpeeds(float leftSpeed, float rightSpeed) noexcept
{
    motors_.setSpeeds(leftSpeed, rightSpeed);
}

void DriveSubsystem::drive(float throttle, float steering) noexcept
{
    motors_.drive(throttle, steering);
}

void DriveSubsystem::brake() noexcept
{
    motors_.brake();
}

void DriveSubsystem::coast() noexcept
{
    motors_.coast();
}

float DriveSubsystem::getLeftRpm() const noexcept
{
    return leftEncoder_.getRpm();
}

float DriveSubsystem::getRightRpm() const noexcept
{
    return rightEncoder_.getRpm();
}

float DriveSubsystem::getAverageRpm() const noexcept
{
    return (leftEncoder_.getRpm() + rightEncoder_.getRpm()) * 0.5f;
}

float DriveSubsystem::getLeftSpeedMps() const noexcept
{
    return leftEncoder_.getLinearVelocity();
}

float DriveSubsystem::getRightSpeedMps() const noexcept
{
    return rightEncoder_.getLinearVelocity();
}

float DriveSubsystem::getAverageSpeedMps() const noexcept
{
    return (leftEncoder_.getLinearVelocity() + rightEncoder_.getLinearVelocity()) * 0.5f;
}

float DriveSubsystem::getDistanceMeters() const noexcept
{
    return (leftEncoder_.getDistanceMeters() + rightEncoder_.getDistanceMeters()) * 0.5f;
}

void DriveSubsystem::resetDistance() noexcept
{
    leftEncoder_.resetTicks();
    rightEncoder_.resetTicks();
}

float DriveSubsystem::getLeftCommandedSpeed() const noexcept
{
    return motors_.a().getSpeed();
}

float DriveSubsystem::getRightCommandedSpeed() const noexcept
{
    return motors_.b().getSpeed();
}

} // namespace robot
