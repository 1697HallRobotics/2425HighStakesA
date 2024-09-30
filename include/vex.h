#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <string>
#include <sstream>

#include "v5.h"
#include "v5_vcs.h"

#include "recording.h"

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

// The brain
brain Brain = brain();

// The motor's base speed.
const float speed = 1.0f;

// define your global instances of motors and other devices here
motor leftBackMotor = motor(PORT2, false);
motor leftFrontMotor = motor(PORT5, false);
motor rightBackMotor = motor(PORT4, false);
motor rightFrontMotor = motor(PORT3, false);

motor_group leftMotors = motor_group(leftBackMotor, leftFrontMotor);
motor_group rightMotors = motor_group(rightBackMotor, rightFrontMotor);