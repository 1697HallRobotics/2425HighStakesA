#include <cstdlib>
#include <cstdio>
#include <cmath>

#include "v5.h"
#include "v5_vcs.h"

using namespace vex;

#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++

// A global instance of competition
competition Competition = competition();

// The controller
controller Controller = controller();
// The controller's deadzone
const float deadzone = 10.1f;

// The motor's base speed.
const float speed = 1.0f;

// define your global instances of motors and other devices here
motor leftBackMotor = motor(0);
motor leftFrontMotor = motor(1);
motor rightBackMotor = motor(2);
motor rightFrontMotor = motor(3);

motor_group leftMotors = motor_group(leftBackMotor, leftFrontMotor);
motor_group rightMotors = motor_group(rightBackMotor, rightFrontMotor);