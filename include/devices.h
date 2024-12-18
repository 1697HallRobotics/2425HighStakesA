#ifndef _DEVICES_H_
#define _DEVICES_H_

#include "macros.h"

#define MACRO_WALLGOAL MACRO_SLOT0

using namespace pros;

// The controller
Controller controller(E_CONTROLLER_MASTER);

// The controller's deadzone
const float deadzone = 10.1f;

// The motor's base speed, as a multiplier.
const float speed = 0.65f;
// negative = reversed motor
const int8_t leftBackPort     = -16;
const int8_t leftFrontPort    = -10;
const int8_t rightBackPort    = -3;
const int8_t rightFrontPort   = -11;
// every port past here is outdated since we have none of these
const int8_t liftPort         = 0;
const int8_t intakePort       = 14;
const int8_t liftIntakePort   = 0; // TODO
const int8_t rampPort         = 0; // TODO
const int8_t clampPort        = 16;

const int8_t distanceSensorPort = 8;

Motor leftBackMotor   (leftBackPort);
Motor leftFrontMotor  (leftFrontPort);
Motor rightBackMotor  (rightBackPort);
Motor rightFrontMotor (rightFrontPort);
Motor liftMotor       (liftPort);
Motor rampMotor       (rampPort);
Motor intakeMotor     (intakePort);
Motor liftIntakeMotor (liftIntakePort);

MotorGroup leftMotors ({leftBackPort, leftFrontPort});
MotorGroup rightMotors({rightBackPort, rightFrontPort});

adi::Pneumatics clampPneumatics = adi::Pneumatics('a', false);

#endif