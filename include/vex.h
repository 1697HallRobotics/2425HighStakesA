#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>
#include <sstream>
#include "api.h"

using namespace pros;

// The controller
Controller controller(E_CONTROLLER_MASTER);

// The controller's deadzone
const float deadzone = 10.1f;

// The motor's base speed, as a multiplier.
const float speed = 1.0f;


const int8_t leftBackPort   = 11;
const int8_t leftFrontPort  = 5;
const int8_t rightBackPort  = 6;
const int8_t rightFrontPort = 3;


/*
const int8_t leftBackPort   = -20;
const int8_t leftFrontPort  = -11;
const int8_t rightBackPort  = -18;
const int8_t rightFrontPort = -1;
*/

const int8_t mysteryPort = 14;

Motor leftBackMotor   = Motor(leftBackPort);
Motor leftFrontMotor  = Motor(leftFrontPort);
Motor rightBackMotor  = Motor(rightBackPort);
Motor rightFrontMotor = Motor(rightFrontPort);

Motor mysteryMotor    = Motor(mysteryPort);

MotorGroup leftMotors  = MotorGroup({leftBackPort, leftFrontPort});
MotorGroup rightMotors = MotorGroup({rightBackPort, rightFrontPort});